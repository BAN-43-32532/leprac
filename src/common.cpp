#include "common.h"

#include <atlstr.h>
#include <string>
#include <Windows.h>

namespace leprac {
// CStringA uses char
std::string toString(std::wstring const& wstr) {
  return CStringA(wstr.c_str()).GetString();
}

// CStringW uses wchar_t
std::wstring toWstring(std::string const& str) {
  return CStringW(str.c_str()).GetString();
}

// I don't think caption str should be localized
void errorBox(std::string const& text, std::string const& caption) {
  MessageBoxW(
    nullptr, toWstring(text).c_str(), toWstring(caption).c_str(), MB_ICONERROR
  );
}

bool is32bit(libmem::Process const& process) {
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process.pid);
  BOOL isWow64;
  IsWow64Process(hProcess, &isWow64);
  CloseHandle(hProcess);
  return isWow64;
}
}  // namespace leprac
