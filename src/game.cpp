#include "game.hpp"

#include <format>
#include <libmem/libmem.hpp>
#include <optional>
#include <Windows.h>
#include <winternl.h>

#include "aho_corasick.hpp"
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
    if (auto process = libmem::FindProcess(toProcessName(gameID).c_str())) { result.emplace_back(gameID); }
  }
  return result;
}

std::string Game::versionToLiteral(std::string version) {
  auto pos = version.find('_');
  if (pos == std::string_view::npos) { return version; }
  version[pos] = '.';
  return version;
}

std::string Game::versionToTag(std::string version) {
  auto pos = version.find('.');
  if (pos == std::string_view::npos) { return version; }
  version[pos] = '_';
  return version;
}

std::optional<bool> Game::isVersionSupported(GameID id, std::string const& version) {
  auto keyGame = toLower(me::enum_name(id));
  if (auto game = Asset::version().at(keyGame); game.contains(version)) { return game.at(version).as_boolean(); }
  return std::nullopt;
}

std::string Game::loadBinary(std::string const& path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  Logger::throwIf(!file, "Failed to open file {}. Check if it is occupied.", path);
  auto size = file.tellg();
  Logger::throwIf(size <= 0, "Failed to get the size of file {} (got nonpositive size).", path);
  file.seekg(0, std::ios::beg);

  std::string data(size, '\0');
  Logger::throwIf(!file.read(data.data(), size), "Failed to read file {}.", path);
  return data;
}

std::optional<GameID> Game::searchIDFromEXE(std::string const& path) {
  static auto trie = [] {
    aho_corasick::trie trie;
    for (auto sv: me::enum_names<GameID>()) { trie.insert(std::string(sv)); }
    return std::move(trie.only_whole_words());
  }();
  try {
    auto data    = loadBinary(path);
    auto results = trie.parse_text(data, ParseMode::StopAtFirstMatch);
    Logger::throwIf(results.size() != 1, "Found {} GameID", results.size());
    return me::enum_cast<GameID>(results[0].get_keyword());
  } catch (...) {}
  Logger::error("Failed to find the game ID of exe file {}", path);
  return std::nullopt;
}

std::string Game::searchVersionFromEXE(GameID id, std::string const& path) {
  static std::unordered_map<GameID, aho_corasick::trie> tries;
  if (!tries.contains(id)) {
    auto  nameID = toLower(me::enum_name(id));
    auto& trie   = tries[id];
    for (auto str: Asset::version().at(nameID).as_table() | views::keys) {
      str              = versionToLiteral(str);
      std::string wstr = toUTF16(str);
      trie.insert(str);
      trie.insert(wstr);
    }
    trie.only_whole_words();
  }
  try {
    auto data   = loadBinary(path);
    auto result = tries[id].parse_text(data, ParseMode::StopAtFirstMatch);
    Logger::throwIf(result.size() != 1, "Found {} version", result.size());
    auto key = result[0].get_keyword();
    if (ranges::contains(Asset::version().at(toLower(me::enum_name(id))).as_table() | views::keys, key)) { return key; }
    return toUTF8(key);
  } catch (...) {}

  Logger::error("Failed to find the game version of exe file {}", path);
  return {};
}

std::vector<fs::path> Game::scanGameEXE(fs::path const& path) {
  static auto nameDefault = [] {
    std::unordered_set<std::string> result;
    for (auto name: me::enum_names<GameID>()) { result.emplace(std::string(name).append(".exe")); }
    return result;
  }();
  std::vector<fs::path> result;
  for (auto const& entry: fs::recursive_directory_iterator(path)) {
    if (!entry.is_regular_file()) continue;
    auto path = entry.path();
    if (path.extension() != ".exe") continue;
    if (nameDefault.contains(path.filename().string())) {
      result.emplace_back(path);
      continue;
    }
    if (fs::file_size(path) >= 20 * 1 << 20) continue;  // Too large
    if (path.filename().string().ends_with("_config.exe")) continue;
    fs::path dir = path.parent_path();
    if (!fs::exists(dir / "dat.led")) continue;
    if (searchIDFromEXE(path.string()).has_value()) { result.emplace_back(path); }
  }
  return result;
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
      if (int len = GetWindowTextLength(hwnd); len > 0) {
        std::wstring title(len + 1, L'\0');
        GetWindowText(hwnd, &title[0], len + 1);
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
  auto status = NtQueryInformationThread(hThread, static_cast<THREADINFOCLASS>(0), &tbi, sizeof(tbi), nullptr);
  CloseHandle(hThread);
  if (status != 0) {
    Logger::error("NtQueryInformationThread failed with status: {:#x}", status);
    return std::nullopt;
  }
  Logger::debug("TEB Base Address: {}", tbi.TebBaseAddress);

  auto tebData = libmem::ReadMemory<TEB>(&process, reinterpret_cast<libmem::Address>(tbi.TebBaseAddress));
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

std::optional<libmem::Address>
Game::getStackAddress(libmem::Process const& process, libmem::Address address, std::vector<uint8_t> const& pattern) {
  auto                     hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process.pid);
  MEMORY_BASIC_INFORMATION mbi;

  auto     base   = address;
  uint64_t offset = 0;
  while (offset <= 0x10000000) {
    if (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(base), &mbi, sizeof(mbi))) {
      // Logger::log(Logger::Level::Error ,
      //   "Region Base: {}, Size: {:#x}, Type: {:#x}, Protect: {:#x}",
      //   mbi.BaseAddress,
      //   mbi.RegionSize,
      //   mbi.Type,
      //   mbi.Protect
      // );
      if (mbi.Protect == 0x04) {
        if (auto result = libmem::DataScan(&process, pattern, base, mbi.RegionSize)) { return *result - 0x04; }
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
