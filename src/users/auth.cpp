#include "auth.hpp"

#include <random>
#include <vector>
#include <memory>

#include <userver/crypto/hash.hpp>
#include <userver/utils/encoding/hex.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <jwt-cpp/jwt.h>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

//TODO: search for user_id in the db

std::string ens::users::GenerateSalt() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8_t> dis(0, 255);
  std::vector<uint8_t> salt(ens::users::SALT_SIZE);
  for (auto &byte : salt) {
    byte = dis(gen);
  }
  return userver::utils::encoding::ToHex(salt.data(), salt.size());
}

// Hash password with passed salt
std::unique_ptr<ens::users::PwdPair> ens::users::HashPwd(const std::string &password, const std::string &salt) {
  std::string salted_password = password + salt;
  auto hash = userver::crypto::hash::Sha512(salted_password);
  PwdPair pwd_pair = PwdPair(hash, salt);
  return std::make_unique<PwdPair>(pwd_pair);
}

// Hash password with random salt
std::unique_ptr<ens::users::PwdPair> ens::users::HashPwd(const std::string &password) {
  std::string salt = GenerateSalt();
  std::string salted_password = password + salt;
  auto hash = userver::crypto::hash::Sha512(salted_password);
  PwdPair pwd_pair = PwdPair(hash, salt);
  return std::make_unique<PwdPair>(pwd_pair);
}

std::unique_ptr<ens::users::JwtPair> ens::users::JwtManager::GenerateJwtPair(const std::string &user_id) {
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
  return std::make_unique<ens::users::JwtPair>(access_token, refresh_token);
}

userver::yaml_config::Schema ens::users::JwtManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: Component for jwt verification logic
    additionalProperties: false
    properties: {}
  )");
}

// Verify jwt and return user id
boost::uuids::uuid ens::users::JwtManager::VerifyJwt(const std::string &token) {
  auto verifier = jwt::verify()
      .allow_algorithm(jwt::algorithm::hs256{this->_secdist_config._jwt_secret});
  try {
    auto decoded = jwt::decode(token);
    verifier.verify(decoded);
    return boost::lexical_cast<boost::uuids::uuid>(decoded.get_payload_claim("user_id").as_string());
  }
  catch (const std::invalid_argument &e) {
    throw JwtVerificationException{};
  }
  catch (const jwt::error::signature_verification_exception &e) {
    throw JwtVerificationException{};
  }
  catch (const boost::bad_lexical_cast &e) {
    throw JwtVerificationException{};
  }
}

void ens::users::AppendJwtManager(userver::components::ComponentList &component_list) {
  component_list.Append<JwtManager>();
}
