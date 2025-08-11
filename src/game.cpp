#include "game.hpp"

#include <format>
#include <libmem/libmem.hpp>
#include <magic_enum/magic_enum_all.hpp>
#include <optional>
#include <Windows.h>
#include <winternl.h>

#include "asset.hpp"
#include "logger.hpp"

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

std::string toProcessName(GameID game) {
  // TODO: Confirm if the names are correct
  return std::format("{}.exe", me::enum_name(game));
}

// void Game::init() {}

auto Game::getID() const { return gameId_; }

// auto Game::process() const { return gameProcess_; }

std::vector<GameID> Game::detectRunningGames() {
  std::vector<GameID> result;
  for (auto gameID: me::enum_values<GameID>()) {
    if (auto process = libmem::FindProcess(toProcessName(gameID).c_str())) {
      result.emplace_back(gameID);
    }
  }
  return result;
}

std::string Game::versionToLiteral(std::string version) {
  auto pos = version.find('_');
  if (pos == std::string_view::npos) { return version; }
  version[pos] = '.';
  return version;
}

std::optional<bool>
Game::isVersionSupported(GameID id, std::string const& version) {
  auto keyGame = toLower(me::enum_name(id));
  if (auto game = Asset::version().at(keyGame); game.contains(version)) {
    return game.at(version).as_boolean();
  }
  return std::nullopt;
}

std::vector<char> Game::loadBinary(std::string const& path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    Logger::error("Failed to open file {}. Check if it is occupied.", path);
    return {};
  }
  auto size = file.tellg();
  if (size <= 0) {
    Logger::error(
      "Failed to get the size of file {} (got nonpositive size).", path
    );
    return {};
  }
  file.seekg(0, std::ios::beg);

  std::vector<char> data(size);
  if (!file.read(data.data(), size)) {
    Logger::error("Failed to read file {}.", path);
    return {};
  }
  return data;
}

std::optional<GameID> Game::searchIDFromEXE(std::string const& path) {
  auto data = loadBinary(path);
  if (data.empty()) {
    Logger::error("Failed to find the game ID of exe file {}", path);
    return std::nullopt;
  }
  for (auto [id, sv]: me::enum_entries<GameID>()) {
    std::boyer_moore_horspool_searcher searcher(sv.begin(), sv.end());
    if (std::search(data.begin(), data.end(), searcher) != data.end()) {
      Logger::debug("{} found (str)", sv);
      return id;
    }
    Logger::debug("{} not found (str)", sv);
  }
  Logger::error("Failed to find the game ID of exe file {}", path);
  return std::nullopt;
}

std::string Game::searchVersionFromEXE(GameID id, std::string const& path) {
  auto data = loadBinary(path);
  if (data.empty()) {
    Logger::error("Failed to find the game version of exe file {}", path);
    return {};
  }
  auto gameTag = toLower(me::enum_name(id));
  Logger::debug("{}", gameTag);
  for (auto str:
       Asset::version().at(gameTag).as_table() | views::keys | views::reverse) {
    str = versionToLiteral(str);
    std::string wstr(
      reinterpret_cast<char const*>(toWstring(str).data()),
      str.size() * sizeof(wchar_t)
    );
    std::boyer_moore_horspool_searcher searcher(wstr.begin(), wstr.end());
    auto it = std::search(data.begin(), data.end(), searcher);
    if (it != data.end()) {
      Logger::debug("{} found (str)", str);
      return str;
    }
    Logger::debug("{} not found (str)", str);
    searcher = std::boyer_moore_horspool_searcher(str.begin(), str.end());
    it       = std::search(data.begin(), data.end(), searcher);
    if (it != data.end()) {
      Logger::debug("{} found (wstr)", str);
      return str;
    }
    Logger::debug("{} not found (wstr)", str);
  }
  Logger::error("Failed to find the game version of exe file {}", path);
  return {};
}

bool Game::completeGameInfo() {
  // if (auto process = libmem::FindProcess(toProcessName(gameId_).c_str())) {
  //   gameProcess_ = *process;
  //   return true;
  // }
  Logger::error("{} process not found.", toProcessName(gameId_));
  return false;
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
    Logger::error("No windows found for PID: {}", process.pid);
    return std::nullopt;
  }
  for (auto const& title: titles) {
    if (title.contains(L".ver")) { return title; }
  }
  Logger::error("No window seems to be the game");
  return std::nullopt;
}

std::optional<TEB> getTEB(libmem::Process const& process) {
  libmem::Thread           mainThread = *libmem::GetThread(&process);
  THREAD_BASIC_INFORMATION tbi{};

  auto hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, mainThread.tid);
  if (!hThread) {
    Logger::error("OpenThread failed, error: {}", GetLastError());
    return std::nullopt;
  }
  auto status = NtQueryInformationThread(
    hThread, static_cast<THREADINFOCLASS>(0), &tbi, sizeof(tbi), nullptr
  );
  CloseHandle(hThread);
  if (status != 0) {
    Logger::error("NtQueryInformationThread failed with status: {:#x}", status);
    return std::nullopt;
  }
  Logger::debug("TEB Base Address: {}", tbi.TebBaseAddress);

  auto tebData = libmem::ReadMemory<TEB>(
    &process, reinterpret_cast<libmem::Address>(tbi.TebBaseAddress)
  );
  if (!tebData) {
    Logger::error("ReadMemory failed, error: {}", GetLastError());
    return std::nullopt;
  }
  uint64_t stackBase  = tebData->StackBase;
  uint64_t stackLimit = tebData->StackLimit;
  Logger::debug("StackBase: {:#x}", stackBase);
  Logger::debug("StackLimit: {:#x}", stackLimit);
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

  // print_hex(combined);
  return combined;
}

std::optional<libmem::Address> Game::getStackAddress(
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
      // Logger::log(Logger::Level::Error ,
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
      Logger::error("VirtualQueryEx failed: {}", GetLastError());
      CloseHandle(hProcess);
      return std::nullopt;
    }
  }
  return std::nullopt;
}
}  // namespace leprac
