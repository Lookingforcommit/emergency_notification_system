#include "auth.hpp"

#include <random>
#include <vector>
#include <memory>

#include <userver/crypto/hash.hpp>
#include <userver/utils/encoding/hex.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/query.hpp>
#include <jwt-cpp/jwt.h>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "schemas/schemas.hpp"
#include "userver/storages/postgres/cluster_types.hpp"


std::string ens::auth::GenerateSalt() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8_t> dis(0, 255);
  std::vector<uint8_t> salt(ens::auth::SALT_SIZE);
  for (auto &byte : salt) {
    byte = dis(gen);
  }
  return userver::utils::encoding::ToHex(salt.data(), salt.size());
}

// Hash password with passed salt
std::unique_ptr<ens::auth::PwdPair> ens::auth::HashPwd(const std::string &password, const std::string &salt) {
  std::string salted_password = password + salt;
  auto hash = userver::crypto::hash::Sha512(salted_password);
  PwdPair pwd_pair = PwdPair(hash, salt);
  return std::make_unique<PwdPair>(pwd_pair);
}

// Hash password with random salt
std::unique_ptr<ens::auth::PwdPair> ens::auth::HashPwd(const std::string &password) {
  std::string salt = GenerateSalt();
  std::string salted_password = password + salt;
  auto hash = userver::crypto::hash::Sha512(salted_password);
  PwdPair pwd_pair = PwdPair(hash, salt);
  return std::make_unique<PwdPair>(pwd_pair);
}

std::unique_ptr<schemas::JWTPair> ens::auth::JWTManager::GenerateJWTPair(const std::string &user_id) {
  auto access_token = jwt::create()
      .set_type("JWS")
      .set_payload_claim("user_id", jwt::claim(user_id))
      .set_issued_now()
      .set_expires_in(std::chrono::seconds{JWT_ACCESS_TOKEN_EXPIRATION})
      .sign(jwt::algorithm::hs256{this->_secdist_config._jwt_secret});
  auto refresh_token = jwt::create()
      .set_type("JWS")
      .set_payload_claim("user_id", jwt::claim(user_id))
      .set_issued_now()
      .set_expires_in(std::chrono::seconds{JWT_REFRESH_TOKEN_EXPIRATION})
      .sign(jwt::algorithm::hs256{this->_secdist_config._jwt_secret});
  return std::make_unique<schemas::JWTPair>(access_token, refresh_token);
}

userver::yaml_config::Schema ens::auth::JWTManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: Component for jwt verification logic
    additionalProperties: false
    properties: {}
  )");
}

// Verify jwt and return user id
boost::uuids::uuid ens::auth::JWTManager::VerifyJWT(const std::string &token) {
  auto verifier = jwt::verify()
      .allow_algorithm(jwt::algorithm::hs256{this->_secdist_config._jwt_secret});
  try {
    auto decoded = jwt::decode(token);
    verifier.verify(decoded);
    boost::uuids::uuid user_id = boost::lexical_cast<boost::uuids::uuid>(decoded.get_payload_claim("user_id").as_string());
    const userver::storages::postgres::Query stored_user_exists_query{
        "SELECT EXISTS ( "
        "SELECT 1 "
        "FROM ens_schema.user "
        "WHERE user_id = $1)",
    };
    userver::storages::postgres::ResultSet
        select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                          stored_user_exists_query,
                                          user_id);
    if (not select_res.AsSingleRow<bool>()) {
      throw UserNotFoundException{boost::uuids::to_string(user_id)};
    }
    return user_id;
  }
  catch (const std::invalid_argument &e) { // invalid jwt format
    throw JWTVerificationException{};
  }
  catch (const jwt::error::signature_verification_exception &e) {
    throw JWTVerificationException{};
  }
  catch (const jwt::error::token_verification_exception &e) { // token expired
    throw JWTVerificationException{};
  }
  catch (const boost::bad_lexical_cast &e) { // invalid user_id
    throw JWTVerificationException{};
  }
}

void ens::auth::AppendJWTManager(userver::components::ComponentList &component_list) {
  component_list.Append<JWTManager>();
}

