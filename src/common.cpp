#include "common.hpp"

#include <atlstr.h>
#include <string>
#include <Windows.h>

namespace leprac {
// CStringA uses char
std::string toString(std::wstring const& wstr) { return CStringA(wstr.c_str()).GetString(); }

// CStringW uses wchar_t
std::wstring toWstring(std::string const& str) { return CStringW(str.c_str()).GetString(); }

std::string toUTF16(std::string const& str) {
  return {reinterpret_cast<char const*>(toWstring(str).data()), str.size() * sizeof(wchar_t)};
}

std::string toUTF8(std::string const& str) {
  return toString({reinterpret_cast<wchar_t const*>(str.data()), str.size() / sizeof(wchar_t)});
}

std::string toLower(std::string_view sv) {
  return sv | views::transform([](unsigned char c) { return std::tolower(c); }) | ranges::to<std::string>();
}

std::string toUpper(std::string_view sv) {
  return sv | views::transform([](unsigned char c) { return std::toupper(c); }) | ranges::to<std::string>();
}

std::string capitalize(std::string_view sv) {
  std::string s = toLower(sv);
  if (!s.empty()) { s[0] = std::toupper(static_cast<unsigned char>(s[0])); }
  return s;
}

// I don't think caption str should be localized
void errorBox(std::string const& text, std::string const& caption) {
  MessageBox(nullptr, toWstring(text).c_str(), toWstring(caption).c_str(), MB_ICONERROR);
}

bool is32bit(libmem::Process const& process) {
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process.pid);
  BOOL isWow64;
  IsWow64Process(hProcess, &isWow64);
  CloseHandle(hProcess);
  return isWow64;
}
}  // namespace leprac
