#pragma once
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <print>
#include <string>
#include <vector>

struct Settings {
  std::vector<std::string> watchList;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, watchList)

inline std::filesystem::path settingsPath() {
#ifdef _WIN32
  return std::filesystem::path(std::getenv("APPDATA")) / "tenkit" /
         "settings.json";
#else
  return std::filesystem::path(std::getenv("HOME")) / ".config" / "tenkit" /
         "settings.json";
#endif
}

inline std::optional<Settings> loadSettings() {
  Settings settings;
  nlohmann::json settings_data;
  std::filesystem::path path = settingsPath();

  if (!std::filesystem::exists(path)) {
    // std::println(stderr, "Setting file not found.");
    return std::nullopt;
  } else {
    std::ifstream f(path.string());

    try {
      settings_data = nlohmann::json::parse(f);
    } catch (const nlohmann::json::parse_error &e) {
      std::println(stderr, "JSONパースエラー: {}", e.what());
      return std::nullopt;
    }

    settings.watchList = settings_data["watchList"];

    return settings;
  }
}

inline bool saveSettings(const std::string &city) {
  const auto nullable_s = loadSettings();
  Settings s;
  if (nullable_s) {
    s = nullable_s.value();
  }

  s.watchList.push_back(city);

  std::filesystem::path path = settingsPath();
  std::filesystem::path dir = settingsPath().parent_path();

  std::filesystem::create_directories(dir);

  std::ofstream out(path);
  if (!out) {
    int err = errno;

    std::error_code ec(err, std::generic_category());

    std::println(stderr, "{} を開けませんでした. 理由: {} (コード: {})",
               path.string(), ec.message(), err);
    return false;
  } else {
    nlohmann::json settings_in_json = s;
    std::string settings_in_string = settings_in_json.dump(4);
    std::print(out, "{}", settings_in_string);
    return true;
  }
}