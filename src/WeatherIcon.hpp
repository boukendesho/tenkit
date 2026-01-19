#pragma once

#include <map>
#include <string>
#include <vector>

/**
 * @brief 天気のテキストアイコン(weather text icon)
 * 情報源: https://wttr.in/
 * 天気の言葉 -> アイコン
 */
const std::map<std::string, std::vector<std::string>> weather_icon = {
    {"不明",{
        "             ",
        "    .-.      ",
        "     __)     ",
        "    (        ",
        "     `-’     ",
        "      •      ",
        "             ",
    }},
    {"晴れ",{
        "    \\   /   ",
        "     .-.     ",
        "  ― (   ) ―  ",
        "     `-’     ",
        "    /   \\   "
    }},
    {"晴時々曇",{
        "             ",
        "   \\  /     ",
        " _ /\"\".-.  ",
        "   \\_(   ). ",
        "   /(___(__) ",
        "             ",
        "             ",
    }},
    {"曇り",{
        "             ",
        "             ",
        "     .--.    ",
        "  .-(    ).  ",
        " (___.__)__) ",
        "             ",
        "             ",
    }},
    {"雨",{
        "             ",
        " _`/\"\".-.  ",
        "  ,\\_(   ). ",
        "   /(___(__) ",
        "     ' ' ' ' ",
        "    ' ' ' '  ",
        "             ",
    }},
    // {"大雨", {
    //     "             ",
    //     " _`/\"\".-.  ",
    //     "  ,\\_(   ). ",
    //     "   /(___(__) ",
    //     "   ‚'‚'‚'‚'  ",
    //     "   ‚'‚'‚'‚'  ",
    //     "             ",
    // }},
    {"雪", {
        "             ",
        "     .-.     ",
        "    (   ).   ",
        "   (___(__)  ",
        "    *  *  *  ",
        "   *  *  *   ",
        "             ",
    }},
    // {"大雪", {
    //     "             ",
    //     "     .-.     ",
    //     "    (   ).   ",
    //     "   (___(__)  ",
    //     "   * * * *   ",
    //     "  * * * *    ",
    //     "             ",
    // }},
    {"雷雨", {
        "             ",
        "     .-.     ",
        "    (   ).   ",
        "   (___(__)  ",
        "   ⚡\"\"⚡\"\"",
        "  ‚'‚'‚'‚'   ",
        "             ",
    }},
    {"霧", {
        "             ",
        "             ",
        " _ - _ - _ - ",
        "  _ - _ - _  ",
        " _ - _ - _ - ",
        "             ",
        "             ",
    }}
};
/**
 * @brief 天気の言葉をアイコンに変換し、stringを返す。
 * 見つからない場合は"不明"を返す。
 */
inline std::vector<std::string> getWeatherIcon(const std::string &weather) {
  std::string deduction_weather;

  // 日本語には天気の言葉が非常に多いので、全ての言葉に対するテキストアイコンを描くことができないため、
  // APIからの天気言葉を主な天気言葉に推測してみる。
  if (weather.contains("晴") && weather.contains("曇")) {
    deduction_weather = "晴時々曇";
  } else if (weather.contains("雨")) {
    deduction_weather = "雨";
  } else if (weather.contains("雪")) {
    deduction_weather = "雪";
  } else if (weather.contains("雷")) {
    deduction_weather = "雷雨";
  } else if (weather.contains("霧")) {
    deduction_weather = "霧";
  } else {
    deduction_weather = weather;
  }

  if (auto it = weather_icon.find(deduction_weather);
      it != weather_icon.end()) {
    return it->second;
  } else {
    return weather_icon.at("不明");
  }
}