module;
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
module update;
import common;

namespace leprac {
using json = nlohmann::json;

constexpr auto repoApiUrl = "https://api.github.com/repos/BAN-43-32532/leprac/releases/latest";
constexpr auto userAgent  = "leprac-updater";

static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  static_cast<std::string*>(userp)->append((char*) contents, size * nmemb);
  return size * nmemb;
}

std::string Update::fetchLatestReleaseJson() {
  CURL* curl = curl_easy_init();
  if (!curl) return {};

  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, repoApiUrl);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  return (res == CURLE_OK) ? response : "";
}

std::string Update::parseLatestVersion(std::string_view jsonStr) {
  try {
    auto j = json::parse(jsonStr);
    if (j.contains("tag_name")) {
      auto ver = j["tag_name"].get<std::string>();
      if (!ver.empty() && (ver[0] == 'v' || ver[0] == 'V')) { ver.erase(0, 1); }
      return ver;
    }
  } catch (...) {
    // JSON parse error
  }
  return {};
}

void Update::notifyUpdate(std::string_view latestVersion, std::string_view url) {
  auto msg = std::format(
    "Update available!\n"
    "Current version: {}\n"
    "Latest version: {}\n"
    "Download: {}\n",
    VERSION,
    latestVersion,
    url
  );
  std::cout << msg;
}

void Update::checkForUpdate() {
  auto jsonStr = fetchLatestReleaseJson();
  if (jsonStr.empty()) {
    std::cerr << "Failed to fetch release info.\n";
    return;
  }

  auto latest = parseLatestVersion(jsonStr);
  if (latest.empty()) {
    std::cerr << "Failed to parse latest version.\n";
    return;
  }

  if (latest != VERSION) {
    auto        j   = json::parse(jsonStr);
    std::string url = j["html_url"].get<std::string>();
    notifyUpdate(latest, url);
  } else {
    std::cout << "You're using the latest version: " << latest << "\n";
  }
}
}  // namespace leprac
