#ifndef UPDATE_HPP
#define UPDATE_HPP
#include <string>

namespace leprac {
class Update {
 public:
  static void checkForUpdate();

 private:
  static std::string fetchLatestReleaseJson();
  static std::string parseLatestVersion(std::string const& jsonStr);
  static void        notifyUpdate(std::string const& latestVersion, std::string const& url);
};
}  // namespace leprac
#endif  // UPDATE_HPP
