#pragma once

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "user/auth.hpp"
#include "schemas/schemas.hpp"
#include "utils/utils.hpp"

namespace ens::user {

// Component for user management logic
class UserManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "user-manager";
  UserManager(const userver::components::ComponentConfig &config,
              const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _jwt_manager(component_context.FindComponent<ens::user::JwtManager>()),
      _pg_cluster(
          component_context
              .FindComponent<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
              .GetCluster()) {}
  static userver::yaml_config::Schema GetStaticConfigSchema();
  std::unique_ptr<ens::user::JwtPair> Create(const std::string &name, const std::string &password);
  [[nodiscard]] std::unique_ptr<ens::user::JwtPair> Login(const std::string &name, const std::string &password) const;
  void ModifyUser(const boost::uuids::uuid &user_id, const schemas::User &new_data);
  [[nodiscard]] std::unique_ptr<ens::user::JwtPair> RefreshToken(const boost::uuids::uuid &user_id) const;
  void DeleteUser(const boost::uuids::uuid &user_id);

 private:
  JwtManager &_jwt_manager;
  userver::storages::postgres::ClusterPtr _pg_cluster;
};

void AppendUserManager(userver::components::ComponentList &component_list);

class UserAlreadyExistsException : public std::exception {
 private:
  const std::string FORMAT{"User already exists name={}"};
  const std::string _msg;
 public:
  UserAlreadyExistsException(const std::string &name) : _msg(fmt::format(this->FORMAT, name)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class UserNotFoundException : public std::exception {
 private:
  const std::string FORMAT{"User does not exist name={}"};
  const std::string _msg;
 public:
  UserNotFoundException(const std::string &name) : _msg(fmt::format(this->FORMAT, name)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class IncorrectPwdException : public std::exception {
 private:
  const std::string FORMAT{"Incorrect password user_id={}"};
  const std::string _msg;
 public:
  IncorrectPwdException(const std::string &user_id) : _msg(fmt::format(this->FORMAT, user_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

}  // namespace ens::user