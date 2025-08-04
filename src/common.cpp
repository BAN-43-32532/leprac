#include "common.h"

#include <chrono>
#include <format>
#include <string>
#include <Windows.h>

namespace leprac {
std::string to_utf8(std::wstring const& wstr) {
  if (wstr.empty()) return "";
  int size = WideCharToMultiByte(
    CP_UTF8,
    0,
    wstr.c_str(),
    static_cast<int>(wstr.size()),
    nullptr,
    0,
    nullptr,
    nullptr
  );
  std::string utf8_str(size, 0);
  WideCharToMultiByte(
    CP_UTF8,
    0,
    wstr.c_str(),
    static_cast<int>(wstr.size()),
    &utf8_str[0],
    size,
    nullptr,
    nullptr
  );
  return utf8_str;
}

bool is32bit(libmem::Process const& process) {
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process.pid);
  BOOL isWow64{};
  IsWow64Process(hProcess, &isWow64);
  CloseHandle(hProcess);
  return isWow64;
}

std::string timestamp() {
  return std::format(
    "{:%Y-%m-%d %H:%M:%S}",
    floor<std::chrono::seconds>(std::chrono::system_clock::now())
  );
}
}  // namespace leprac
