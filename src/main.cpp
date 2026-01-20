#include "AreaMap.hpp"
#include "Display.hpp"
#include "GeoIPDetect.hpp"
#include "Settings.hpp"
#include <CLI/CLI.hpp>
#include <cpr/cpr.h>
#include <cpr/response.h>
#include <expected>
#include <format>
#include <print>
#include <string>
#include <vector>

// C++23の新機能 expected を使ってみた
std::expected<cpr::Response, std::string>
queryWeather(const std::string &city_input) {

  auto result = getCityCode(city_input);

  if (!result) {
    return std::unexpected(std::format("エラー: {}", result.error()));
  } else {
    std::string cityCode = *result;
    cpr::Response r = cpr::Get(cpr::Url{
        "https://weather.tsukumijima.net/api/forecast/city/" + cityCode});

    if (r.status_code == 200) {
      return r;
    } else {
      return std::unexpected(std::format("APIエラー: code {}, 地名: {}",
                                         r.status_code, city_input));
    }
  }
}

int main(int argc, char **argv) {

  std::string city_input;
  std::string city_to_watch;
  std::vector<std::string> city_list;
  std::vector<std::string> query_args;
  bool auto_detect = false;
  bool show_saved_city_list = false;
  int days_to_show = 3;

  auto st = loadSettings();

  CLI::App app{"天気予報情報が取得できるシンプルなツール"};

  // デフォルトのラベルを日本語に上書きする
  app.set_help_flag("-h,--help", "ヘルプを表示");
  app.get_formatter()->label("OPTIONS", "オプション");
  app.get_formatter()->label("USAGE", "使い方");
  app.get_formatter()->label("Positionals", "引数");

  app.add_option("-q,--query", query_args, "調べたい地域 [日数学(オプション)]")
      ->expected(1, 2);
  app.add_option("-w,--watch", city_to_watch, "保存したい地域");
  app.add_flag("-l,--list", show_saved_city_list, "保存される地域リスト");

  CLI11_PARSE(app, argc, argv);

  int active_options =
      (app.count("-q") > 0) + (app.count("-l") > 0) + (app.count("-w") > 0);

  if (active_options > 1) {
    std::println(stderr,
                 "エラー: -q, -l, -w "
                 "は同時に使用できません。どれか一つを選んでください。");
    return 1;
  }

  if (app.count("-w") > 0) {
    // まず、その都市がエリアマップに存在するかどうかを確認する
    auto validation = getCityCode(city_to_watch);
    if (!validation) {
      std::println(stderr, "エラー: {}", validation.error());
      return 1;
    }

    std::string city_name_jp = getCityNameJP(city_to_watch, *validation);

    // 重複を避けるために、すでにリストに含まれているかどうかを確認する
    if (st) {
      auto it =
          std::find(st->watchList.begin(), st->watchList.end(), city_name_jp);
      if (it == st->watchList.end()) {
        st->watchList.push_back(city_name_jp);
      } else {
        std::println("'{}' は既にリストに含まれています。", city_name_jp);
        return 0;
      }
    }

    // ディスクに保存する
    if (saveSettings(city_name_jp)) {
      std::println("'{}' をリストに保存しました。", city_name_jp);
    } else {
      std::println(stderr, "エラー: 設定の保存に失敗しました。");
      return 1;
    }

    // 保存して終了
    return 0;
  }

  // ユーザーの入力がなかったらIPアドレス自動検知になること。
  if (app.count_all() == 1 && !show_saved_city_list) {
    if (st && !st->watchList.empty()) {
      city_list = st->watchList;
      std::println("保存された地域の天気を表示します...");
    } else {
      auto_detect = true;
    }
  }

  if (app.count("-q") > 0) {
    city_input = query_args[0];
    if (query_args.size() > 1) {
      try {
        days_to_show = std::stoi(query_args[1]);
      } catch (const std::invalid_argument &e) {
        std::println(stderr, "エラー: '{}' は有効な数字ではありません。",
                     query_args[1]);
        return 1;
      }
    }
    auto resp = queryWeather(city_input);
    if (!resp) {
      std::println("{}", resp.error());
      return 1;
    }

    display(*resp, days_to_show);
  }

  if (show_saved_city_list) {
    if (st->watchList.empty() || !st) {
      std::println(
          "保存された地域はありません。'-w' オプションで追加してください。");
    } else {
      std::println("--- 保存済みの地域リスト ---");
      // C++23 enumerateでシリアル番号を表示
      for (const auto &[index, city] : st->watchList | std::views::enumerate) {
        std::println("[{}] {}", index + 1, city);
      }
    }
    return 0; // Listを表示して終了
  }

  if (auto_detect) {
    std::print("自動地域検索中...");

    // IPアドレスから自動判明する機能
    GeoLocation loc = detectLocation();

    if (loc.success) {
      if (loc.country != "Japan") {
        std::println(stderr,
                     "本ツールは日本の地名しか使えません. 自動判明した国は {}.",
                     loc.country);
        return 1;
      }
      std::println("判明した地域: {}", loc.city);
      city_input = loc.city;

      auto resp = queryWeather(city_input);
      if (!resp) {
        std::println("{}", resp.error());
        return 1;
      }

      display(*resp, days_to_show);
    } else {
      std::println(stderr, "自動地域検索失敗しました. "
                           "インターネットの繋がりをチェックしてください.");
      return 1;
    }
  }

  // 保存された地域が存在したら、１つずつ出力する
  // stやcity_listがnullの場合動きません
  for (auto const &city : city_list) {
    auto resp = queryWeather(city);
    if (!resp) {
      std::println("{}", resp.error());
      return 1;
    }

    // 今日だけの天気を出力する
    days_to_show = 1;

    display(*resp, days_to_show);
  }

  return 0;
}