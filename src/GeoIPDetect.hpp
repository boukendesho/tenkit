#pragma once
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>

struct GeoLocation {
  std::string status;
  std::string country;
  std::string city;
  bool success = false;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(GeoLocation, status, country, city)
};

inline GeoLocation detectLocation() {
  GeoLocation loc;

  // ip-api経由で地域名を取得する (English name, 例: Osaka)
  cpr::Response geo_r = cpr::Get(cpr::Url{"http://ip-api.com/json/"});
  if (geo_r.status_code == 200) {
    auto gl = nlohmann::json::parse(geo_r.text).get<GeoLocation>();
    if (gl.status == "success") {
      loc.city = gl.city;
      loc.country = gl.country;
      loc.success = true;
    }
  }
  return loc;
};