#include "handlers.hpp"

#include <string>
#include <memory>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_error.hpp>
#include <userver/formats/json/schema.hpp>
#include <boost/uuid/uuid.hpp>

#include "user.hpp"
#include "auth.hpp"
#include "schemas/schemas.hpp"

// TODO: catch generic json_parse errors and throw 422

userver::formats::json::Value ens::user::UserCreateHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &,
                                                                                   const userver::formats::json::Value &request_json,
                                                                                   userver::server::request::RequestContext &) const {
  try {
    schemas::User user_data = request_json.As<schemas::User>();
    std::unique_ptr<schemas::JWTPair> jwt_pair = this->_user_manager.Create(user_data.name, user_data.password);
    schemas::JWTPair returned_pair{jwt_pair->access_token, jwt_pair->refresh_token};
    return schemas::Serialize(returned_pair, userver::formats::serialize::To<userver::formats::json::Value>());
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

void ens::user::AppendUserCreateHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserCreateHandler>();
}

userver::formats::json::Value ens::user::UserLoginHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &,
                                                                                  const userver::formats::json::Value &request_json,
                                                                                  userver::server::request::RequestContext &) const {
  try {
    schemas::User user_data = request_json.As<schemas::User>();
    std::unique_ptr<schemas::JWTPair> jwt_pair = this->_user_manager.Login(user_data.name, user_data.password);
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

void ens::user::AppendUserLoginHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserLoginHandler>();
}
userver::formats::json::Value ens::user::UserModifyHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                   const userver::formats::json::Value &request_json,
                                                                                   userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    schemas::User new_data = request_json.As<schemas::User>();
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
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
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::user::AppendUserModifyHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserModifyHandler>();
}

userver::formats::json::Value ens::user::UserRefreshTokenHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                         const userver::formats::json::Value &,
                                                                                         userver::server::request::RequestContext &) const {
  const std::string &refresh_token = request.GetHeader("Authorization");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(refresh_token);
    std::unique_ptr<schemas::JWTPair> jwt_pair = this->_user_manager.RefreshToken(user_id);
    userver::formats::json::ValueBuilder builder;
    builder["access_token"] = jwt_pair->access_token;
    builder["refresh_token"] = jwt_pair->refresh_token;
    return builder.ExtractValue();
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::user::AppendUserRefreshTokenHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserRefreshTokenHandler>();
}

userver::formats::json::Value ens::user::UserDeleteHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                   const userver::formats::json::Value &,
                                                                                   userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
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
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::user::AppendUserDeleteHandler(userver::components::ComponentList &component_list) {
  component_list.Append<UserDeleteHandler>();
}
