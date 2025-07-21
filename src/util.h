#ifndef UTIL_H
#define UTIL_H

#include <string>

#include "libmem/libmem.hpp"

namespace leprac {
std::string to_utf8(std::wstring const &wstr);
bool        is32bit(libmem::Process const &process);
}  // namespace leprac

#endif  // UTIL_H
