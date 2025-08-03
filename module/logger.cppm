module;
#include <format>
#include <fstream>
#include <ostream>
#include <Windows.h>
export module leprac.logger;

import leprac.common;

export namespace leprac {
class Logger {
 public:
  Logger() = delete;
  enum class Level { Info, Warn, Error, Debug };
  static void init();
  static void deinit();

  template<class... Args>
  static void log(Level lvl, std::format_string<Args...> fmt, Args&&... args) {
    if (lvl == Level::Debug && !debug_) return;
    std::print(ofs_, "{}", prefix(lvl));
    std::println(ofs_, fmt, std::forward<Args>(args)...);
    flush();
  }

  template<class... Args>
  static void info(std::format_string<Args...> fmt, Args&&... args) {
    log(Level::Info, fmt, std::forward<Args>(args)...);
  }

  template<class... Args>
  static void warn(std::format_string<Args...> fmt, Args&&... args) {
    log(Level::Warn, fmt, std::forward<Args>(args)...);
  }

  template<class... Args>
  static void error(std::format_string<Args...> fmt, Args&&... args) {
    log(Level::Error, fmt, std::forward<Args>(args)...);
  }

  template<class... Args>
  static void debug(std::format_string<Args...> fmt, Args&&... args) {
    log(Level::Debug, fmt, std::forward<Args>(args)...);
  }

  template<class... Args>
  static void throwError(std::format_string<Args...> fmt, Args&&... args) {
    auto msg = std::format(fmt, std::forward<Args>(args)...);
    error("{}", msg);
    deinit();
    throw std::runtime_error(msg);
  }

  static void syncDebug();

 private:
  static std::string prefix(Level lvl);
  static void        flush();

  inline static bool          debug_{};
  inline static bool          console_{};
  inline static std::ofstream ofs_;
};
}  // namespace leprac

class ConsoleManager {
 public:
  static void Attach() {
    SetConsoleOutputCP(CP_UTF8);
    if (attached_) return;
    if (!AllocConsole()) return;

    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);
    attached_ = true;
  }

  static void Detach() {
    if (!attached_) return;
    FreeConsole();
    attached_ = false;
  }

 private:
  static inline bool attached_ = false;
};
