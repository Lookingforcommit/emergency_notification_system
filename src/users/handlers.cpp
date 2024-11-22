#include "handlers.hpp"

#include <string>
#include <memory>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/exceptions.hpp>

#include "users.hpp"
#include "auth.hpp"
#include "schemas/schemas.hpp"

// TODO: Add a json schema for jwt pairs
userver::formats::json::Value ens::users::UserCreateHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &,
                                                                                    const userver::formats::json::Value &request_json,
                                                                                    userver::server::request::RequestContext &) const {
  const std::string &name = request_json["name"].As<std::string>();
  const std::string &password = request_json["password"].As<std::string>();
  try {
    std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_user_manager.Create(name, password);
    userver::formats::json::ValueBuilder builder;
    builder["access_token"] = jwt_pair->access_token;
    builder["refresh_token"] = jwt_pair->refresh_token;
    return builder.ExtractValue();
  }
  catch (const UserAlreadyExistsException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kConflictState,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::users::AppendUserCreateHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserCreateHandler>();
}

userver::formats::json::Value ens::users::UserLoginHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &,
                                                                                   const userver::formats::json::Value &request_json,
                                                                                   userver::server::request::RequestContext &) const {
  const std::string &name = request_json["name"].As<std::string>();
  const std::string &password = request_json["password"].As<std::string>();
  try {
    std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_user_manager.Login(name, password);
    userver::formats::json::ValueBuilder builder;
    builder["access_token"] = jwt_pair->access_token;
    builder["refresh_token"] = jwt_pair->refresh_token;
    return builder.ExtractValue();
  }
  catch (const UserNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const IncorrectPwdException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::users::AppendUserLoginHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserLoginHandler>();
}

//TODO: catch incorrect jsons and throw 422
userver::formats::json::Value ens::users::UserModifyHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                    const userver::formats::json::Value &request_json,
                                                                                    userver::server::request::RequestContext &) const {
  schemas::User new_data = request_json.As<schemas::User>();
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_user_manager.ModifyUser(user_id, new_data);
    return userver::formats::json::Value{};
  }
  catch (const UserNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::users::AppendUserModifyHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserModifyHandler>();
}

userver::formats::json::Value ens::users::UserRefreshTokenHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                          const userver::formats::json::Value &,
                                                                                          userver::server::request::RequestContext &) const {
  const std::string &refresh_token = request.GetHeader("Authorization");
  try {
    std::string user_id = _jwt_verif_manager.VerifyJwt(refresh_token);
    std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_user_manager.RefreshToken(user_id);
    userver::formats::json::ValueBuilder builder;
    builder["access_token"] = jwt_pair->access_token;
    builder["refresh_token"] = jwt_pair->refresh_token;
    return builder.ExtractValue();
  }
  catch (const JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::users::AppendUserRefreshTokenHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserRefreshTokenHandler>();
}

userver::formats::json::Value ens::users::UserDeleteHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                    const userver::formats::json::Value &,
                                                                                    userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_user_manager.DeleteUser(user_id);
    return userver::formats::json::Value{};
  }
  catch (const UserNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::users::AppendUserDeleteHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserDeleteHandler>();
}
