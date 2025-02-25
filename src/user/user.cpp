#include "user.hpp"

#include <string>
#include <memory>

#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/query.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/transaction.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/utils/boost_uuid7.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "user/auth.hpp"
#include "schemas/schemas.hpp"

//TODO : user credentials should be non-loggable

userver::yaml_config::Schema ens::user::UserManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: Component for user management logic
    additionalProperties: false
    properties: {}
  )");
}

// Create a user in the DB and return a pair of jwt tokens
std::unique_ptr<schemas::JWTPair> ens::user::UserManager::Create(const std::string &name,
                                                                 const std::string &password) {
  const userver::storages::postgres::Query create_user_query{
      "INSERT INTO ens_schema.user (user_id, name, password_hash, password_salt) "
      "VALUES ($1, $2, $3, $4) "
      "ON CONFLICT (name) DO NOTHING"
  };
  userver::storages::postgres::Transaction insert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  boost::uuids::uuid user_id = userver::utils::generators::GenerateBoostUuidV7();
  std::unique_ptr<ens::auth::PwdPair> pwd_pair = ens::auth::HashPwd(password);
  userver::storages::postgres::ResultSet insert_res = insert_transaction.Execute(create_user_query,
                                                                                 user_id,
                                                                                 name,
                                                                                 pwd_pair->hashed_password,
                                                                                 pwd_pair->salt);
  if (not insert_res.RowsAffected()) {
    throw UserAlreadyExistsException{name};
  }
  insert_transaction.Commit();
  std::unique_ptr<schemas::JWTPair> jwt_pair = this->_jwt_manager.GenerateJWTPair(boost::uuids::to_string(user_id));
  return jwt_pair;
}

// Login into an existing account and retrieve jwt tokens
// May log in by old credentials due to unfinished ModifyUser
std::unique_ptr<schemas::JWTPair> ens::user::UserManager::Login(const std::string &name,
                                                                const std::string &password) const {
  const userver::storages::postgres::Query stored_user_info_query{
      "SELECT user_id, password_hash, password_salt "
      "FROM ens_schema.user "
      "WHERE name = $1",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        stored_user_info_query,
                                        name);
  if (select_res.IsEmpty()) {
    throw UserNotFoundException{name};
  }
  userver::storages::postgres::Row user_data = select_res[0];
  std::string user_id = boost::uuids::to_string(user_data["user_id"].As<boost::uuids::uuid>());
  ens::auth::PwdPair stored_pair{user_data["password_hash"].As<std::string>(),
                      user_data["password_salt"].As<std::string>()};
  std::unique_ptr<ens::auth::PwdPair> login_pair = ens::auth::HashPwd(password, stored_pair.salt);
  if (stored_pair.hashed_password != login_pair->hashed_password) {
    throw IncorrectPwdException{user_id};
  }
  std::unique_ptr<schemas::JWTPair> jwt_pair = this->_jwt_manager.GenerateJWTPair(user_id);
  return jwt_pair;
}

// Modify existing user data
void ens::user::UserManager::ModifyUser(const boost::uuids::uuid &user_id, const schemas::User &new_data) {
  const userver::storages::postgres::Query update_user_query{
      "UPDATE ens_schema.user "
      "SET name = $1, password_hash = $2, password_salt = $3 "
      "WHERE user_id = $4",
  };
  userver::storages::postgres::Transaction update_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  std::unique_ptr<ens::auth::PwdPair> pwd_pair = ens::auth::HashPwd(new_data.password);
  userver::storages::postgres::ResultSet update_res = update_transaction.Execute(update_user_query,
                                                                                 new_data.name,
                                                                                 pwd_pair->hashed_password,
                                                                                 pwd_pair->salt,
                                                                                 user_id);
  if (not update_res.RowsAffected()) {
    throw UserNotFoundException{boost::uuids::to_string(user_id)};
  }
  update_transaction.Commit();
}

// Get new jwt tokens
std::unique_ptr<schemas::JWTPair> ens::user::UserManager::RefreshToken(const boost::uuids::uuid &user_id) const {
  std::unique_ptr<schemas::JWTPair> jwt_pair = this->_jwt_manager.GenerateJWTPair(boost::uuids::to_string(user_id));
  return jwt_pair;
}

// Delete an account
void ens::user::UserManager::DeleteUser(const boost::uuids::uuid &user_id) {
  const userver::storages::postgres::Query delete_user_query{
      "DELETE "
      "FROM ens_schema.user "
      "WHERE user_id = $1",
  };
  userver::storages::postgres::Transaction delete_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet delete_res = delete_transaction.Execute(delete_user_query,
                                                                                 user_id);
  if (not delete_res.RowsAffected()) {
    throw UserNotFoundException{boost::uuids::to_string(user_id)};
  }
  delete_transaction.Commit();
}

void ens::user::AppendUserManager(userver::components::ComponentList &component_list) {
  component_list.Append<UserManager>();
}
