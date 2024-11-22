#pragma once

#include <memory>
#include <string>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>

#include "users/users.hpp"

namespace ens::users {
class UserJsonHandlerBase : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  UserJsonHandlerBase(const userver::components::ComponentConfig &config,
                      const userver::components::ComponentContext &context)
      : HttpHandlerJsonBase(config, context),
        _user_manager(context.FindComponent<UserManager>()) {}
 protected:
  UserManager &_user_manager;
};

class UserCreateHandler : public UserJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-create";
  using UserJsonHandlerBase::UserJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendUserCreateHandler(userver::components::ComponentList &component_list);

class UserLoginHandler : public UserJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-login";
  using UserJsonHandlerBase::UserJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendUserLoginHandler(userver::components::ComponentList &component_list);

class UserModifyHandler : public UserJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-modifyUser";
  UserModifyHandler(const userver::components::ComponentConfig &config,
                    const userver::components::ComponentContext &context)
      : UserJsonHandlerBase(config, context),
        _jwt_verif_manager(context.FindComponent<JwtManager>()) {}
  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request, const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext &) const override;
 private:
  JwtManager &_jwt_verif_manager;
};

void AppendUserModifyHandler(userver::components::ComponentList &component_list);

class UserRefreshTokenHandler : public UserJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-refreshToken";
  UserRefreshTokenHandler(const userver::components::ComponentConfig &config,
                          const userver::components::ComponentContext &context)
      : UserJsonHandlerBase(config, context),
        _jwt_verif_manager(context.FindComponent<JwtManager>()) {}
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
 private:
  JwtManager &_jwt_verif_manager;
};

void AppendUserRefreshTokenHandler(userver::components::ComponentList &component_list);

class UserDeleteHandler : public UserJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-delete";
  UserDeleteHandler(const userver::components::ComponentConfig &config,
                    const userver::components::ComponentContext &context)
      : UserJsonHandlerBase(config, context),
        _jwt_verif_manager(context.FindComponent<JwtManager>()) {}
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
 private:
  JwtManager &_jwt_verif_manager;
};

void AppendUserDeleteHandler(userver::components::ComponentList &component_list);

}
