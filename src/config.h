#ifndef CONFIG_H
#define CONFIG_H

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <toml++/toml.hpp>

class Config {
public:
    // Initialize configuration: read or create default file
    static void init() {
        const std::string filename = "leprac.cfg";
        if (!std::filesystem::exists(filename)) {
            // Create default table with comments
            toml::table cfg;
            cfg.comment("This is a config file for leprac using TOML format");

            // language
            cfg.insert("language", toml::value(std::string("")));
            cfg["language"]->comment(
                "A string (\"en\" / \"zh\" / \"ja\") that defines in which language the launcher and the in-game overlay gui are"
            );

            // paths to executables
            cfg.insert("path_le01", toml::value(std::string("")));
          cfg["path_le01"].comments
            cfg["path_le01"]->comment("A string of absolute / relative path to your game executable Le01");
            cfg.insert("path_le02", toml::value(std::string("")));
            cfg["path_le02"]->comment("A string of absolute / relative path to your game executable Le02");
            cfg.insert("path_le03", toml::value(std::string("")));
            cfg["path_le03"]->comment("A string of absolute / relative path to your game executable Le03");
            cfg.insert("path_le04", toml::value(std::string("")));
            cfg["path_le04"]->comment("A string of absolute / relative path to your game executable Le04");

            // fonts list
            cfg.insert("path_fonts", toml::array{});
            cfg["path_fonts"]->comment(
                "A list of strings of absolute / relative path to your font ttf / ttc file"
            );

            // debug mode
            cfg.insert("debug_mode", toml::value(false));
            cfg["debug_mode"]->comment("Debug mode (true / false)");

            // Write out
            std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
            ofs << cfg;
            ofs.close();
        }

        // Parse file
        auto table = toml::parse_file(filename);
        language = toml::find_or(table, "language", std::string(""));
        pathLe01 = toml::find_or(table, "path_le01", std::string(""));
        pathLe02 = toml::find_or(table, "path_le02", std::string(""));
        pathLe03 = toml::find_or(table, "path_le03", std::string(""));
        pathLe04 = toml::find_or(table, "path_le04", std::string(""));

        // Load fonts list
        pathFonts.clear();
        if (auto arr = table["path_fonts"].as_array()) {
            for (auto& item : *arr) {
                if (item.is_string()) {
                    pathFonts.emplace_back(item.value<std::string>());
                }
            }
        }

        debugMode = toml::find_or(table, "debug_mode", false);
    }

    // Getters
    static const std::string& getLanguage() { return language; }
    static const std::string& getPathLe01() { return pathLe01; }
    static const std::string& getPathLe02() { return pathLe02; }
    static const std::string& getPathLe03() { return pathLe03; }
    static const std::string& getPathLe04() { return pathLe04; }
    static const std::vector<std::string>& getPathFonts() { return pathFonts; }
    static bool getDebugMode() { return debugMode; }

    // Setters
    static void setLanguage(const std::string& v) { language = v; }
    static void setPathLe01(const std::string& v) { pathLe01 = v; }
    static void setPathLe02(const std::string& v) { pathLe02 = v; }
    static void setPathLe03(const std::string& v) { pathLe03 = v; }
    static void setPathLe04(const std::string& v) { pathLe04 = v; }
    static void setPathFonts(const std::vector<std::string>& v) { pathFonts = v; }
    static void setDebugMode(bool v) { debugMode = v; }

private:
    // Configuration values
    static inline std::string language = "";
    static inline std::string pathLe01 = "";
    static inline std::string pathLe02 = "";
    static inline std::string pathLe03 = "";
    static inline std::string pathLe04 = "";
    static inline std::vector<std::string> pathFonts = {};
    static inline bool debugMode = false;
};


#endif //CONFIG_H
