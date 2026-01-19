#include "AreaMap.hpp"
#include "GeoIPDetect.hpp"
#include "WeatherData.hpp"
#include "WeatherIcon.hpp"
#include <CLI/CLI.hpp>
#include <cpr/cpr.h>
#include <format>
#include <print>
#include <string>

int main(int argc, char **argv) {

  // システムの現在の時刻を取得する
  // auto now = std::chrono::system_clock::now();
  // auto local_time = std::chrono::current_zone()->to_local(now);
  // std::chrono::hh_mm_ss hms{local_time.time_since_epoch() %
  //                           std::chrono::days{1}};

  // int current_hour = hms.hours().count();
  // std::println("Hours: {}", current_hour);

  CLI::App app{"天気予報情報が取得できるシンプルなツール"};

  std::string city_input; // ユーザーの入力がなかったら、ユーザーのIPアドレスから自動判明する
  bool auto_detect = false;

  app.add_option("-q,--query", city_input, "調べたい地名");

  CLI11_PARSE(app, argc, argv);

  // ユーザーの入力がなかったらIPアドレス自動検知になること。
  if (app.count("-q") == 0) {
    auto_detect = true;
  }

  GeoLocation loc = detectLocation(); // IPアドレスから自動判明する機能

  if (auto_detect) {
    std::print("自動地域検索中...");
    if (loc.success) {
      if (loc.country != "Japan") {
        std::println(stderr,
                     "本ツールは日本の地名しか使えません. 自動判明した国は {}.",
                     loc.country);
        return 1;
      }
      std::println("判明した地域: {}", loc.city);
      city_input = loc.city;
    } else {
      std::println(stderr, "自動地域検索失敗しました. "
                           "インターネットの繋がりをチェックしてください.");
      return 1;
    }
  }

  // C++23 新機能 expected を使う
  auto result = getCityCode(city_input);

  if (!result) {
    std::println(stderr, "エラー: {}", result.error());
    return 1;
  }

  std::string cityCode = *result;

  // 天気情報を検知するリクエスト
  cpr::Response r = cpr::Get(cpr::Url{
      "https://weather.tsukumijima.net/api/forecast/city/" + cityCode});

  if (r.status_code == 200) {
    // http 応答 -> json オブジェクト -> 天気struct
    auto data = nlohmann::json::parse(r.text).get<WeatherResponse>();

    std::println("{}", data.title);

    std::vector<std::string> weather_layout;

    for (const auto &f : data.forecasts) {
      // 今日分の降水確率や最低・最高気温や天気詳細のうち、過ぎた分（例・12時に API にアクセスしたときの0時～6時の降水確率）は -- または null になる。
      std::string min_t = f.temperature.min.celsius.value_or("--");
      std::string max_t = f.temperature.max.celsius.value_or("--");

      std::string weather_in_detail = f.detail.weather.value_or("情報なし");
      std::string wind = f.detail.wind.value_or("情報なし");
      std::string wave = f.detail.wave.value_or("情報なし");

      auto icon = getWeatherIcon(f.telop);

      weather_layout.push_back(std::format("{:^15}", f.dateLabel));
      weather_layout.push_back(icon[0]);
      weather_layout.push_back(
          std::format("{:<15} 天気: {}", icon[1], f.telop));
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
  } else {
    std::println(stderr, "APIエラー: code {}, 地名: {}", r.status_code,
                 city_input);
  }

  return 0;
}