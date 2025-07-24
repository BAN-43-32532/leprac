#include "game.h"

#include <Windows.h>
#include <winternl.h>

#include "magic_enum/magic_enum_utility.hpp"

namespace leprac {
struct THREAD_BASIC_INFORMATION {
  NTSTATUS  ExitStatus;
  PVOID     TebBaseAddress;
  CLIENT_ID ClientId;
  KAFFINITY AffinityMask;
  KPRIORITY Priority;
  KPRIORITY BasePriority;
};

struct TEB {
  uint64_t Reserved1;
  uint64_t StackBase;
  uint64_t StackLimit;
};

std::string toProcessName(GameId game) {
  // TODO: Confirm if the names are correct
  switch (game) {
  case GameId::Le01: return "Le01.exe";
  case GameId::Le02: return "Le02.exe";
  case GameId::Le03: return "Le03.exe";
  case GameId::Le04: return "Le04.exe";
  case GameId::Uso : return "Uso.exe";
  default          : throw std::invalid_argument("Invalid GameId");
  }
}

std::vector<GameInfo> Game::detectRunningGame() {
  std::vector<GameInfo> result;
  magic_enum::enum_for_each<GameId>([&](auto val) {
    GameId gameId  = val;
    auto   process = libmem::FindProcess(toProcessName(gameId).c_str());
    if (process) { result.emplace_back(gameId, *process); }
  });
  return result;
}

bool Game::completeGameInfo() {
  auto main_thread = libmem::GetThread(&gameInfo_.process);
  if (!main_thread) { return false; }
  gameInfo_.main_thread = *main_thread;
  return true;
}

std::vector<std::wstring> getWindowTitles(libmem::Process const& process) {
  std::vector<std::wstring> result;

  struct EnumContext {
    DWORD                      pid;
    std::vector<std::wstring>* titles;
  } context{process.pid, &result};

  auto callback = [](HWND hwnd, LPARAM lParam) {
    auto ctx = reinterpret_cast<EnumContext*>(lParam);

    DWORD windowPid = 0;
    GetWindowThreadProcessId(hwnd, &windowPid);
    if (windowPid == ctx->pid) {
      if (int len = GetWindowTextLengthW(hwnd); len > 0) {
        std::wstring title(len + 1, L'\0');
        GetWindowTextW(hwnd, &title[0], len + 1);
        title.resize(len);
        ctx->titles->emplace_back(title);
      }
    }
    return TRUE;
  };
  EnumWindows(callback, reinterpret_cast<LPARAM>(&context));
  return result;
}

// Example title: 連縁无現里　～ Evanescent Existence.ver 1.20a
// Signature: ".ver"
std::optional<std::wstring> Game::getGameTitle(libmem::Process const& process) {
  auto titles = getWindowTitles(process);
  if (titles.empty()) {
    logBuffer.println("No windows found for PID: {}", process.pid);
    return std::nullopt;
  }
  for (auto const& title: titles) {
    if (title.contains(L".ver")) { return title; }
  }
  logBuffer.println("No window seems to be the game");
  return std::nullopt;
}

std::optional<TEB> getTEB(GameInfo gameInfo) {
  logBuffer.println("Tid: {}", gameInfo.main_thread.tid);
  THREAD_BASIC_INFORMATION tbi{};

  auto hThread =
    OpenThread(THREAD_QUERY_INFORMATION, FALSE, gameInfo.main_thread.tid);
  if (!hThread) {
    logBuffer.println("OpenThread failed, error: {}", GetLastError());
    return std::nullopt;
  }
  auto status = NtQueryInformationThread(
    hThread, static_cast<THREADINFOCLASS>(0), &tbi, sizeof(tbi), nullptr
  );
  CloseHandle(hThread);
  if (status != 0) {
    logBuffer.println(
      "NtQueryInformationThread failed with status: {:#x}", status
    );
    return std::nullopt;
  }
  logBuffer.println("TEB Base Address: {}", tbi.TebBaseAddress);

  auto tebData = libmem::ReadMemory<TEB>(
    &gameInfo.process, reinterpret_cast<libmem::Address>(tbi.TebBaseAddress)
  );
  if (!tebData) {
    logBuffer.println("ReadMemory failed, error: {}", GetLastError());
    return std::nullopt;
  }
  uint64_t stackBase  = tebData->StackBase;
  uint64_t stackLimit = tebData->StackLimit;
  logBuffer.println("StackBase: {:#x}", stackBase);
  logBuffer.println("StackLimit: {:#x}", stackLimit);
  return tebData;
}

std::vector<uint8_t> Game::getStackSignature(libmem::Process const& process) {
  std::vector<uint8_t> literal_prefix(20, 0);
  literal_prefix[0] = 0x0C;

  auto targetTitle    = getGameTitle(process);
  auto titleBytes     = reinterpret_cast<uint8_t const*>(targetTitle->c_str());
  auto titleByteCount = targetTitle->size() * sizeof(wchar_t);

  std::vector<uint8_t> combined;
  combined.reserve(literal_prefix.size() + titleByteCount);
  combined.insert(combined.end(), literal_prefix.begin(), literal_prefix.end());
  combined.insert(combined.end(), titleBytes, titleBytes + titleByteCount);

  print_hex(combined);
  return combined;
}

std::optional<libmem::Address> getStackAddress(
  libmem::Process const&      process,
  libmem::Address             address,
  std::vector<uint8_t> const& pattern
) {
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process.pid);
  MEMORY_BASIC_INFORMATION mbi;

  auto     base   = address;
  uint64_t offset = 0;
  while (offset <= 0x10000000) {
    if (VirtualQueryEx(
          hProcess, reinterpret_cast<LPCVOID>(base), &mbi, sizeof(mbi)
        )) {
      // logBuffer.println(
      //   "Region Base: {}, Size: {:#x}, Type: {:#x}, Protect: {:#x}",
      //   mbi.BaseAddress,
      //   mbi.RegionSize,
      //   mbi.Type,
      //   mbi.Protect
      // );
      if (mbi.Protect == 0x04) {
        if (auto result =
              libmem::DataScan(&process, pattern, base, mbi.RegionSize)) {
          return *result - 0x04;
        }
      }
      base    = reinterpret_cast<uint64_t>(mbi.BaseAddress) + mbi.RegionSize;
      offset += mbi.RegionSize;
    } else {
      logBuffer.println("VirtualQueryEx failed: {}", GetLastError());
      CloseHandle(hProcess);
      return std::nullopt;
    }
  }
  return std::nullopt;
}
}  // namespace leprac
