#include "handlers.hpp"

#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_error.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "schemas/schemas.hpp"
#include "user/auth.hpp"


userver::formats::json::Value ens::recipients::RecipientCreateHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                              const userver::formats::json::Value &request_json,
                                                                                              userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    schemas::RecipientWithoutId user_data = request_json.As<schemas::RecipientWithoutId>();
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::RecipientDraft> created_data = this->_recipient_manager.Create(user_id, user_data);
    return schemas::Serialize(*created_data, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::auth::GenericJWTException &e) {
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
  try {
    const boost::uuids::uuid recipient_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("recipient_id"));
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::RecipientWithId> recipient = this->_recipient_manager.GetById(user_id, recipient_id);
    return schemas::Serialize(*recipient, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const boost::bad_lexical_cast &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
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
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::RecipientWithIdList> recipients = this->_recipient_manager.GetAll(user_id);
    userver::formats::json::ValueBuilder recipients_json(userver::formats::common::Type::kArray);
    for (const auto &item : *recipients) {
      recipients_json.PushBack(schemas::Serialize(item,
                                                  userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return recipients_json.ExtractValue();
  }
  catch (const ens::auth::GenericJWTException &e) {
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
  try {
    const boost::uuids::uuid draft_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("draft_id"));
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::RecipientWithId> recipient = this->_recipient_manager.ConfirmCreation(user_id, draft_id);
    return schemas::Serialize(*recipient, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const boost::bad_lexical_cast &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
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
  try {
    const boost::uuids::uuid recipient_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("recipient_id"));
    schemas::RecipientWithoutId user_data = request_json.As<schemas::RecipientWithoutId>();
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::RecipientWithId> modified_data = this->_recipient_manager.ModifyRecipient(user_id,
                                                                                                       recipient_id,
                                                                                                       user_data);
    return schemas::Serialize(*modified_data, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const boost::bad_lexical_cast &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
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
  try {
    const boost::uuids::uuid recipient_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("recipient_id"));
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    this->_recipient_manager.DeleteRecipient(user_id, recipient_id);
    return userver::formats::json::Value{};
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const boost::bad_lexical_cast &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
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
