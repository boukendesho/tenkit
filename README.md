# TenKit
天気予報の情報が取得できるシンプルなツール（日本地名のみ）

![output example](https://github.com/boukendesho/tenkit/blob/main/screenshot.png?raw=true)

## 利用中のAPI:
- 天気情報API: [天気予報 API（livedoor 天気互換）](https://weather.tsukumijima.net/)
- IPアドレス検知API: [ip-api](http://ip-api.com/)

## usage
```
$ ./tenkit -h

天気予報の情報が取得できるシンプルなツール


./tenkit/build/tenkit [オプション]


OPTIONS:
  -h,     --help              ヘルプを表示
  -q,     --query TEXT        調べたい地域 [日数学(オプション)]
  -w,     --watch TEXT        保存したい地域
  -r,     --remove TEXT       リストから地域を削除
  -l,     --list              保存される地域リスト

```

## features
- できる限りc++23 featureを使う
- IPアドレスからユーザーエリア（英語の地名、例：Kyoto）を検出
- 地域コードに基づいて現在または３日間の天気情報が検索可能
- 予報の日付が選択可能（例：tenkit -q kyoto 1/2/3）
- 地域名日本語入力可能
- 降水確率>=70%時, 色づけで強調（TODO）
- 気になる地域名がwatchListに保存できる機能