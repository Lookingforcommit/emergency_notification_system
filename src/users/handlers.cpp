#include "handlers.hpp"

#include <string>
#include <memory>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_error.hpp>
#include <userver/formats/json/schema.hpp>

#include "users.hpp"
#include "auth.hpp"
#include "schemas/schemas.hpp"

// TODO: Add a json schema for jwt pairs
// TODO: catch generic json_parse errors and throw 422
// TODO: convert ids to boost here to catch bad_lexical_cast

userver::formats::json::Value ens::users::UserCreateHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &,
                                                                                    const userver::formats::json::Value &request_json,
                                                                                    userver::server::request::RequestContext &) const {
  try {
    schemas::User user_data = request_json.As<schemas::User>();
    std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_user_manager.Create(user_data.name, user_data.password);
    userver::formats::json::ValueBuilder builder;
    builder["access_token"] = jwt_pair->access_token;
    builder["refresh_token"] = jwt_pair->refresh_token;
    return builder.ExtractValue();
  }
  catch (const userver::formats::json::Exception &e) {
    throw userver::server::http::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kClientError,
        userver::server::http::HttpStatus::kUnprocessableEntity,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
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
  try {
    schemas::User user_data = request_json.As<schemas::User>();
    std::unique_ptr<ens::users::JwtPair> jwt_pair = this->_user_manager.Login(user_data.name, user_data.password);
    userver::formats::json::ValueBuilder builder;
    builder["access_token"] = jwt_pair->access_token;
    builder["refresh_token"] = jwt_pair->refresh_token;
    return builder.ExtractValue();
  }
  catch (const userver::formats::json::Exception &e) {
    throw userver::server::http::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kClientError,
        userver::server::http::HttpStatus::kUnprocessableEntity,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
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
userver::formats::json::Value ens::users::UserModifyHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                    const userver::formats::json::Value &request_json,
                                                                                    userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    schemas::User new_data = request_json.As<schemas::User>();
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_user_manager.ModifyUser(user_id, new_data);
    return userver::formats::json::Value{};
  }
  catch (const userver::formats::json::Exception &e) {
    throw userver::server::http::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kClientError,
        userver::server::http::HttpStatus::kUnprocessableEntity,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
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

// TODO: catch token expired
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
