#include "handlers.hpp"

#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_error.hpp>

#include "schemas/schemas.hpp"
#include "users/auth.hpp"

// TODO: throw error if parameter is missing
// TODO: convert ids to boost here to catch bad_lexical_cast

userver::formats::json::Value ens::recipients::RecipientCreateHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                              const userver::formats::json::Value &request_json,
                                                                                              userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    schemas::RecipientWithoutId user_data = request_json.As<schemas::RecipientWithoutId>();
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    LOG_DEBUG() << "Create user_id=" << user_id;
    std::unique_ptr<schemas::RecipientDraft> created_data = this->_recipient_manager.Create(user_id, user_data);
    return schemas::Serialize(*created_data, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::users::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const userver::formats::json::Exception &e) {
    throw userver::server::http::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kClientError,
        userver::server::http::HttpStatus::kUnprocessableEntity,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::recipients::AppendRecipientCreateHandler(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientCreateHandler>();
}

userver::formats::json::Value ens::recipients::RecipientGetByIdHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                               const userver::formats::json::Value &,
                                                                                               userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string recipient_id = request.GetArg("recipient_id");
  LOG_DEBUG() << "GetById recipient_id=" << recipient_id;
  try {
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientWithId> recipient = this->_recipient_manager.GetById(user_id, recipient_id);
    return schemas::Serialize(*recipient, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::users::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::recipients::AppendRecipientGetByIdHandler(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientGetByIdHandler>();
}

userver::formats::json::Value ens::recipients::RecipientGetAllHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                              const userver::formats::json::Value &,
                                                                                              userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientWithIdList> recipients = this->_recipient_manager.GetAll(user_id);
    userver::formats::json::ValueBuilder recipients_json(userver::formats::common::Type::kArray);
    for (const auto& item : *recipients) {
      recipients_json.PushBack(schemas::Serialize(item, userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return recipients_json.ExtractValue();
  }
  catch (const ens::users::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::recipients::AppendRecipientGetAllHandler(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientGetAllHandler>();
}

userver::formats::json::Value ens::recipients::RecipientConfirmCreationHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                       const userver::formats::json::Value &,
                                                                                                       userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string draft_id = request.GetArg("draft_id");
  try {
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientWithId> recipient = this->_recipient_manager.ConfirmCreation(user_id, draft_id);
    return schemas::Serialize(*recipient, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::users::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const DraftNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::recipients::AppendRecipientConfirmCreationHandler(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientConfirmCreationHandler>();
}

userver::formats::json::Value ens::recipients::RecipientModifyHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                              const userver::formats::json::Value &request_json,
                                                                                              userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string recipient_id = request.GetArg("recipient_id");
  try {
    schemas::RecipientWithoutId user_data = request_json.As<schemas::RecipientWithoutId>();
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientWithId> modified_data = this->_recipient_manager.ModifyRecipient(user_id,
                                                                                                       recipient_id,
                                                                                                       user_data);
    return schemas::Serialize(*modified_data, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::users::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const userver::formats::json::Exception &e) {
    throw userver::server::http::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kClientError,
        userver::server::http::HttpStatus::kUnprocessableEntity,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::recipients::AppendRecipientModifyHandler(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientModifyHandler>();
}

userver::formats::json::Value ens::recipients::RecipientDeleteHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                              const userver::formats::json::Value &,
                                                                                              userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string recipient_id = request.GetArg("recipient_id");
  try {
    std::string user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_recipient_manager.DeleteRecipient(user_id, recipient_id);
    return userver::formats::json::Value{};
  }
  catch (const ens::users::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::recipients::AppendRecipientDeleteHandler(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientDeleteHandler>();
}
