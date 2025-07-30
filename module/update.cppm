module;
#include <string>

export module leprac.update;

export namespace leprac {
class Update {
 public:
  static void checkForUpdate();

 private:
  static std::string fetchLatestReleaseJson();
  static std::string parseLatestVersion(std::string const& jsonStr);
  static void
  notifyUpdate(std::string const& latestVersion, std::string const& url);
};

}  // namespace leprac
