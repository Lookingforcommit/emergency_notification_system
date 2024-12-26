#pragma once

#include <string>

#include "userver/formats/json.hpp"
#include "userver/utils/strong_typedef.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace ens::utils {
const std::string DB_COMPONENT_NAME = "postgres-ens";

class ParsedSecdistConfig {
 public:
  using JwtSecret = userver::utils::NonLoggable<class JwtSecretTag, std::string>;
  const std::string _jwt_secret;
  ParsedSecdistConfig(const userver::formats::json::Value &val)
      : _jwt_secret(val["jwt_secret"].As<JwtSecret>()) {}
};

std::optional<boost::uuids::uuid> optional_str_to_uuid(const std::optional<std::string> &converted);

std::optional<std::string> optional_uuid_to_str(const std::optional<boost::uuids::uuid> &converted);
}