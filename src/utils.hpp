#pragma once

#include <string>

#include <userver/formats/json.hpp>
#include <userver/utils/strong_typedef.hpp>

namespace utils {
const std::string DB_COMPONENT_NAME = "postgres-ens";

class ParsedSecdistConfig {
 public:
  using JwtSecret = userver::utils::NonLoggable<class JwtSecretTag, std::string>;
  const std::string _jwt_secret;
  ParsedSecdistConfig(const userver::formats::json::Value &val)
      : _jwt_secret(val["jwt_secret"].As<JwtSecret>()) {}
};
}