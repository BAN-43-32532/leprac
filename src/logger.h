#ifndef LOG_H
#define LOG_H

#include <format>
#include <SDL3/SDL.h>
#include <sstream>
#include <string>

namespace leprac {
inline class LogBuffer {
  std::ostringstream oss;

 public:
  template<typename T>
  LogBuffer& operator<<(T const& val) {
    oss << val;
    return *this;
  }

  template<typename... Args>
  void append(std::format_string<Args...> fmt, Args&&... args) {
    oss << std::format(fmt, std::forward<Args>(args)...);
  }

  void flush() {
    std::string msg = oss.str();
    if (!msg.empty()) {
      SDL_Log(msg.c_str());
      clear();
    }
  }

  template<typename... Args>
  void println(std::format_string<Args...> fmt, Args&&... args) {
    append(fmt, std::forward<Args>(args)...);
    flush();
  }

  [[nodiscard]] std::string str() const { return oss.str(); }

  void clear() {
    oss.str("");
    oss.clear();
  }

  void print_hex(std::vector<uint8_t> hex) {
    logBuffer.println("Data size: {} bytes:", hex.size());
    logBuffer << "Hex: ";
    for (size_t i = 0; i < hex.size(); ++i) {
      logBuffer.append("{:02X} ", hex[i]);
      if ((i + 1) % 16 == 0 && i < hex.size() - 1) {
        logBuffer.flush();
        logBuffer.append("     ");
      }
    }
    logBuffer.flush();
  }
} logBuffer;
}  // namespace leprac

#endif  // LOG_H
