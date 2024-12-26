#pragma once

#include <memory>
#include <string>

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/secdist/component.hpp>

#include "utils/utils.hpp"

namespace ens::user {
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

struct JwtPair {
  std::string access_token;
  std::string refresh_token;
  JwtPair() = default;
  JwtPair(std::string access_token, std::string refresh_token)
      : access_token(std::move(access_token)), refresh_token(std::move(refresh_token)) {};
};

std::unique_ptr<ens::user::PwdPair> HashPwd(const std::string &password, const std::string &salt);
std::unique_ptr<PwdPair> HashPwd(const std::string &password);
std::string GenerateSalt();

// Component for jwt verification logic
class JwtManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "jwt-manager";
  JwtManager(const userver::components::ComponentConfig &config,
             const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _secdist_config(
          component_context.FindComponent<userver::components::Secdist>().Get().Get<ens::utils::ParsedSecdistConfig>()
      ) {}
  boost::uuids::uuid VerifyJwt(const std::string &token);
  std::unique_ptr<ens::user::JwtPair> GenerateJwtPair(const std::string &user_id);
  static userver::yaml_config::Schema GetStaticConfigSchema();
 private:
  ens::utils::ParsedSecdistConfig _secdist_config;
};

void AppendJwtManager(userver::components::ComponentList &component_list);

class JwtVerificationException : public std::exception {
 private:
  const std::string MSG{"Incorrect or expired jwt token provided"};
 public:
  [[nodiscard]] const char *what() const
  noexcept override { return MSG.c_str(); };
};
}