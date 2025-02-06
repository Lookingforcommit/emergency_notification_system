#pragma once

#include <memory>
#include <string>
#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>

#include "utils/utils.hpp"
#include "schemas/schemas.hpp"

namespace ens::auth {
constexpr int SALT_SIZE = 128;
constexpr int JWT_ACCESS_TOKEN_EXPIRATION = 3600; // seconds
constexpr int JWT_REFRESH_TOKEN_EXPIRATION = 2592000; // seconds

struct PwdPair {
  std::string hashed_password;
  std::string salt;
  PwdPair() = default;
  PwdPair(std::string hashed_password, std::string salt)
      : hashed_password(std::move(hashed_password)), salt(std::move(salt)) {};
};

std::unique_ptr<ens::auth::PwdPair> HashPwd(const std::string &password, const std::string &salt);
std::unique_ptr<PwdPair> HashPwd(const std::string &password);
std::string GenerateSalt();

// Component for jwt verification logic
class JWTManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "jwt-manager";
  JWTManager(const userver::components::ComponentConfig &config,
             const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _secdist_config(
          component_context.FindComponent<userver::components::Secdist>().Get().Get<ens::utils::JWTSecdistConfig>()
      ),
      _pg_cluster(
          component_context
      .FindComponent<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
      .GetCluster()) {}
  boost::uuids::uuid VerifyJWT(const std::string &token);
  std::unique_ptr<schemas::JWTPair> GenerateJWTPair(const std::string &user_id);
  static userver::yaml_config::Schema GetStaticConfigSchema();
 private:
  ens::utils::JWTSecdistConfig _secdist_config;
  userver::storages::postgres::ClusterPtr _pg_cluster;
};

void AppendJWTManager(userver::components::ComponentList &component_list);

class GenericJWTException : public std::exception {
 public:
  const std::string _msg;
  explicit GenericJWTException(const std::string& msg) : _msg(msg) {}
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class JWTVerificationException : public GenericJWTException {
 public:
  JWTVerificationException() : GenericJWTException("Incorrect or expired JWT token provided") {}
};

class UserNotFoundException : public GenericJWTException {
 private:
  static constexpr std::string_view FORMAT{"User does not exist user_id={}"};
 public:
  UserNotFoundException(const std::string &user_id) : GenericJWTException(fmt::format(this->FORMAT, user_id)) {};
};
}