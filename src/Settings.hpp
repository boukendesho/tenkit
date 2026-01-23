#pragma once
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <print>
#include <set>
#include <string>

struct Settings {
  std::set<std::string> watchList;
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
  Settings s;
  nlohmann::json settings_data;
  const auto path = settingsPath();

  if (!std::filesystem::exists(path)) {
    // std::println(stderr, "Setting file not found.");
    return std::nullopt;
  }
  std::ifstream in(path);

  try {
    settings_data = nlohmann::json::parse(in);
  } catch (const nlohmann::json::parse_error &e) {
    std::println(stderr, "JSONパースエラー: {}", e.what());
    return std::nullopt;
  }

  s.watchList = settings_data["watchList"];

  return s;
}

inline std::expected<int, std::string> saveSettings(const Settings &s) {
  const auto path = settingsPath();
  const auto dir = settingsPath().parent_path();
  std::filesystem::create_directories(dir);

  std::ofstream out(path);
  if (!out) {
    int err = errno;

    std::error_code ec(err, std::generic_category());

    return std::unexpected(
        std::format("{} を開けませんでした. 理由: {} (コード: {})",
                    path.string(), ec.message(), err));
  }

  const nlohmann::json settings_in_json = s;
  std::print(out, "{}", settings_in_json.dump(4));

  return 0;
}