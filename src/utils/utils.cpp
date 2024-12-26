#include "utils.hpp"

std::optional<boost::uuids::uuid> ens::utils::optional_str_to_uuid(const std::optional<std::string> &converted) {
  if (not converted.has_value()) {
    return std::nullopt;
  }
  return boost::lexical_cast<boost::uuids::uuid>(converted.value());
}

std::optional<std::string> ens::utils::optional_uuid_to_str(const std::optional<boost::uuids::uuid> &converted) {
  if (not converted.has_value()) {
    return std::nullopt;
  }
  return boost::uuids::to_string(converted.value());
}
