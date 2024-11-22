#pragma once

#include <cstdlib>

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "users/auth.hpp"
#include "schemas/schemas.hpp"
#include "utils.hpp"

namespace ens::users {

// Component for users management logic
class UserManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "user-manager";
  UserManager(const userver::components::ComponentConfig &config,
              const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _jwt_manager(component_context.FindComponent<ens::users::JwtManager>()),
      _pg_cluster(
          component_context
              .FindComponent<userver::components::Postgres>(utils::DB_COMPONENT_NAME)
              .GetCluster()) {}
  static userver::yaml_config::Schema GetStaticConfigSchema();
  std::unique_ptr<ens::users::JwtPair> Create(const std::string &name, const std::string &password);
  [[nodiscard]] std::unique_ptr<ens::users::JwtPair> Login(const std::string &name, const std::string &password) const;
  void ModifyUser(const std::string &user_id, const schemas::User &new_data);
  [[nodiscard]] std::unique_ptr<ens::users::JwtPair> RefreshToken(const std::string &user_id) const;
  void DeleteUser(const std::string &user_id);

 private:
  JwtManager &_jwt_manager;
  userver::storages::postgres::ClusterPtr _pg_cluster;
};

void AppendUserManager(userver::components::ComponentList &component_list);

class UserAlreadyExistsException : public std::exception {
 private:
  const std::string MSG{"User already exists"};
 public:
  [[nodiscard]] const char *what() const
  noexcept override { return MSG.c_str(); };
};

class UserNotFoundException : public std::exception {
 private:
  const std::string MSG{"User does not exist"};
 public:
  [[nodiscard]] const char *what() const
  noexcept override { return MSG.c_str(); };
};

class IncorrectPwdException : public std::exception {
 private:
  const std::string MSG{"Incorrect password"};
 public:
  [[nodiscard]] const char *what() const
  noexcept override { return MSG.c_str(); };
};

}  // namespace ens::users