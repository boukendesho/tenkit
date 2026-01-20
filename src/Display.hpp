#pragma once
#include "WeatherData.hpp"
#include "WeatherIcon.hpp"
#include <cpr/response.h>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>

inline void display(cpr::Response r, int display_type) {
  auto data = nlohmann::json::parse(r.text).get<WeatherResponse>();

  std::println("{}", data.title);

  std::vector<std::string> weather_layout;

  // 1と以下 -> 今日  2 -> 今日と明日  2以上は全部出力する。
  int n = std::clamp(display_type, 1, 3);

  for (const auto &f : data.forecasts | std::views::take(n)) {
    // 今日分の降水確率や最低・最高気温や天気詳細のうち、過ぎた分（例・12時に
    // API にアクセスしたときの0時～6時の降水確率）は -- または null になる。
    // {display}の天気しか出力しない。

    std::string min_t = f.temperature.min.celsius.value_or("--");
    std::string max_t = f.temperature.max.celsius.value_or("--");

    std::string weather_in_detail = f.detail.weather.value_or("情報なし");
    std::string wind = f.detail.wind.value_or("情報なし");
    std::string wave = f.detail.wave.value_or("情報なし");

    auto icon = getWeatherIcon(f.telop);

    weather_layout.push_back(std::format("{:^15}", f.dateLabel));
    weather_layout.push_back(icon[0]);
    weather_layout.push_back(std::format("{:<15} 天気: {}", icon[1], f.telop));
    weather_layout.push_back(
        std::format("{:<15} 気温: {} ~ {}°C", icon[2], min_t, max_t));
    weather_layout.push_back(
        std::format("{:<15} 詳細: {}", icon[3], weather_in_detail));
    weather_layout.push_back(std::format("{:<15} 風強: {}", icon[4], wind));
    weather_layout.push_back(std::format("{:<15} 波高: {}", icon[5], wave));
    weather_layout.push_back(
        std::format("{:<15} 降水: 早朝{} | 午前{} | 午後{} | 夜間{}", icon[6],
                    f.chanceOfRain.T00_06, f.chanceOfRain.T06_12,
                    f.chanceOfRain.T12_18, f.chanceOfRain.T18_24));
    weather_layout.push_back("\n");
  }

  // テキストアイコンと天気情報を共に1行ずつprintする。
  for (const auto &wl : weather_layout) {
    std::println("{}", wl);
  }

  // 最後に天気概況文を出す
  // std::println("まとめ:\n {}",data.description.text);
}