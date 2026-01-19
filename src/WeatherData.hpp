#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

// ソース: https://weather.tsukumijima.net/#response-field
// null可能な項目にはoptionalを使う
// 天気詳細
struct Detail {
    std::optional<std::string> weather;
    std::optional<std::string> wind;
    std::optional<std::string> wave;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Detail, weather, wind, wave)
};

// 気温のタイプとその数値
struct TempValue {
    std::optional<std::string> celsius; // 摂氏 (°C)
    std::optional<std::string> fahrenheit; // 華氏 (°F)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TempValue, celsius, fahrenheit)
};

// 気温
struct Temperature {
    TempValue min; // 最低気温
    TempValue max; // 最高気温
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Temperature, min, max)
};

// 降水確率
struct ChanceOfRain {
    std::string T00_06; // 0 時から 6 時までの降水確率
    std::string T06_12; // 6 時から 12 時までの降水確率
    std::string T12_18; // 12 時から 18 時までの降水確率
    std::string T18_24; // 18 時から 24 時までの降水確率
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ChanceOfRain, T00_06, T06_12, T12_18, T18_24)
};

// 一日の予測データ
struct Forecast {
    std::string date;
    std::string dateLabel; // 予報日（今日・明日・明後日のいずれか）
    std::string telop; // 例: "晴れ"
    Detail detail;
    Temperature temperature;
    ChanceOfRain chanceOfRain;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Forecast, date, dateLabel, telop, detail, temperature, chanceOfRain)
};

// 天気概況文
struct Description {
    std::string text;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Description, text)
};

// 予報を発表した地域を定義
struct Location {
    std::string city; // 地域名（気象観測所名）（例・八幡）
    std::string prefecture; // 都道府県名（例・福岡県）
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Location, city, prefecture)
};

// メタオブジェクト
struct WeatherResponse {
    std::string title;
    Description description;
    Location location;
    std::vector<Forecast> forecasts; // ３日間の天気情報

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WeatherResponse, title, description, location, forecasts)
};