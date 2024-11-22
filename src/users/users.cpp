#include "users.hpp"

#include <string>

#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/query.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/transaction.hpp>
#include <userver/utils/uuid7.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

#include <users/auth.hpp>
#include <schemas/schemas.hpp>

userver::yaml_config::Schema ens::users::UserManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: Component for users management logic
additionalProperties: false
properties: {}
)");
}

// Create a user in the DB and return a pair of jwt tokens
std::unique_ptr<ens::users::JwtPair> ens::users::UserManager::Create(const std::string &name,
                                                                     const std::string &password) {
  const userver::storages::postgres::Query create_user_query{
      "INSERT INTO user (user_id, name, password_hash, password_salt)"
      "VALUES ($1, $2, $3) "
  };
  const userver::storages::postgres::Query user_exists_query{
      "SELECT EXISTS ("
      "SELECT 1"
      "FROM user"
      "WHERE name = ($1)"
      ") AS user_exists_query"
  };
  userver::storages::postgres::Transaction exists_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kSlave, {});
  bool exists_res = exists_transaction.Execute(user_exists_query, name).AsSingleRow<bool>();
  if (exists_res) {
    throw UserAlreadyExistsException{};
  }
  userver::storages::postgres::Transaction insert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  const std::string user_id = userver::utils::generators::GenerateUuidV7();
  std::unique_ptr<PwdPair> pwd_pair = HashPwd(password);
  userver::storages::postgres::ResultSet insert_res = insert_transaction.Execute(create_user_query, user_id, name,
                                                                                 pwd_pair->hashed_password,
                                                                                 pwd_pair->salt);
  insert_transaction.Commit();
  std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_jwt_manager.GenerateJwtPair(user_id);
  return jwt_pair;
}

// Login into an existing account and retrieve jwt tokens
std::unique_ptr<ens::users::JwtPair> ens::users::UserManager::Login(const std::string &name,
                                                                    const std::string &password) const {
  const userver::storages::postgres::Query stored_user_info_query{
      "SELECT user_id, password_hash, password_salt"
      "FROM user"
      "WHERE name = ($1)"
  };
  userver::storages::postgres::Transaction pwd_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kSlave, {});
  userver::storages::postgres::ResultSet select_res = pwd_transaction.Execute(stored_user_info_query, name);
  if (select_res.IsEmpty()) {
    throw UserNotFoundException{};
  }
  userver::storages::postgres::Row user_data = select_res[0];
  std::string user_id = user_data["user_id"].As<std::string>();
  PwdPair stored_pair{user_data["password_hash"].As<std::string>(),
                      user_data["password_salt"].As<std::string>()};
  std::unique_ptr<PwdPair> login_pair = HashPwd(password);
  if (stored_pair.hashed_password != login_pair->hashed_password) {
    throw IncorrectPwdException{};
  }
  std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_jwt_manager.GenerateJwtPair(user_id);
  return jwt_pair;
}

// Modify existing user data
void ens::users::UserManager::ModifyUser(const std::string &user_id, const schemas::User &new_data) {
  const userver::storages::postgres::Query update_user_query{
      "UPDATE user"
      "SET name = ($1), password_hash = ($2), password_salt = ($3))"
      "WHERE user_id = ($4)"
  };
  userver::storages::postgres::Transaction update_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  std::unique_ptr<PwdPair> pwd_pair = HashPwd(new_data.password);
  userver::storages::postgres::ResultSet update_res = update_transaction.Execute(update_user_query, new_data.name,
                                                                                 pwd_pair->hashed_password,
                                                                                 pwd_pair->salt, user_id);
  if (not update_res.RowsAffected()) {
    throw UserNotFoundException{};
  }
  update_transaction.Commit();
}

// Get new jwt tokens
std::unique_ptr<ens::users::JwtPair> ens::users::UserManager::RefreshToken(const std::string &user_id) const {
  std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_jwt_manager.GenerateJwtPair(user_id);
  return jwt_pair;
}

// Delete an account
void ens::users::UserManager::DeleteUser(const std::string &user_id) {
  const userver::storages::postgres::Query delete_user_query{
      "DELETE"
      "FROM user"
      "WHERE user_id = ($1)"
  };
  userver::storages::postgres::Transaction delete_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet delete_res = delete_transaction.Execute(delete_user_query, user_id);
  if (not delete_res.RowsAffected()) {
    throw UserNotFoundException{};
  }
  delete_transaction.Commit();
}

void ens::users::AppendUserManager(userver::components::ComponentList &component_list) {
  component_list.Append<UserManager>();
}
