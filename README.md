# TenKit
天気予報情報が取得できるシンプルなツール

利用中のAPI:
- 天気情報API: [天気予報 API（livedoor 天気互換）](https://weather.tsukumijima.net/)
- IPアドレス検知API: [ip-api](http://ip-api.com/)

# features
- できる限りc++23 featureを使う
- IPアドレスからユーザーエリア（英語の地名、例：Kyoto）を検出
- 地域コードに基づいて現在または３日間の天気情報を検索
- 予報の日付を選択可能（例：tenkit -q kyoto 1/2/3）
- 降水確率>=70%時, 色づけで強調（TODO）
- 気になる地域名をwatchListに保存できる機能