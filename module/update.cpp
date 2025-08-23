module;
#include <string>
export module update;

export namespace leprac {
class Update {
 public:
  static void checkForUpdate();

 private:
  static std::string fetchLatestReleaseJson();
  static std::string parseLatestVersion(std::string_view jsonStr);
  static void        notifyUpdate(std::string_view latestVersion, std::string_view url);
};
}  // namespace leprac
