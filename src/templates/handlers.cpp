#include "handlers.hpp"

#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_error.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "schemas/schemas.hpp"
#include "user/auth.hpp"

userver::formats::json::Value ens::templates::TemplateCreateHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                            const userver::formats::json::Value &request_json,
                                                                                            userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    schemas::NotificationTemplateWithoutId user_data = request_json.As<schemas::NotificationTemplateWithoutId>();
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::NotificationTemplateDraft>
        created_data = this->_template_manager.Create(user_id, user_data);
    return schemas::Serialize(*created_data, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::user::JwtVerificationException &e) {
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

void ens::templates::AppendTemplateCreateHandler(userver::components::ComponentList &component_list) {
  component_list.Append<TemplateCreateHandler>();
}

userver::formats::json::Value ens::templates::TemplateGetByIdHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                             const userver::formats::json::Value &,
                                                                                             userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const boost::uuids::uuid template_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("template_id"));
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::NotificationTemplateWithId>
        notification_template = this->_template_manager.GetById(user_id, template_id);
    return schemas::Serialize(*notification_template, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::user::JwtVerificationException &e) {
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
  catch (const NotificationTemplateNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::templates::AppendTemplateGetByIdHandler(userver::components::ComponentList &component_list) {
  component_list.Append<TemplateGetByIdHandler>();
}

userver::formats::json::Value ens::templates::TemplateGetAllHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                            const userver::formats::json::Value &,
                                                                                            userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::NotificationTemplateWithIdList> templates = this->_template_manager.GetAll(user_id);
    userver::formats::json::ValueBuilder templates_json(userver::formats::common::Type::kArray);
    for (const auto &item : *templates) {
      templates_json.PushBack(schemas::Serialize(item,
                                                 userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return templates_json.ExtractValue();
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::templates::AppendTemplateGetAllHandler(userver::components::ComponentList &component_list) {
  component_list.Append<TemplateGetAllHandler>();
}

userver::formats::json::Value ens::templates::TemplateConfirmCreationHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                     const userver::formats::json::Value &,
                                                                                                     userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const boost::uuids::uuid draft_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("draft_id"));
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::NotificationTemplateWithId>
        notification_template = this->_template_manager.ConfirmCreation(user_id, draft_id);
    return schemas::Serialize(*notification_template, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::user::JwtVerificationException &e) {
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

void ens::templates::AppendTemplateConfirmCreationHandler(userver::components::ComponentList &component_list) {
  component_list.Append<TemplateConfirmCreationHandler>();
}

userver::formats::json::Value ens::templates::TemplateModifyHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                            const userver::formats::json::Value &request_json,
                                                                                            userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const boost::uuids::uuid template_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("template_id"));
  try {
    schemas::NotificationTemplateWithoutId user_data = request_json.As<schemas::NotificationTemplateWithoutId>();
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::NotificationTemplateWithId> modified_data = this->_template_manager.ModifyTemplate(user_id,
                                                                                                                template_id,
                                                                                                                user_data);
    return schemas::Serialize(*modified_data, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::user::JwtVerificationException &e) {
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
  catch (const NotificationTemplateNotFoundException &e) {
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

void ens::templates::AppendTemplateModifyHandler(userver::components::ComponentList &component_list) {
  component_list.Append<TemplateModifyHandler>();
}

userver::formats::json::Value ens::templates::TemplateDeleteHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                            const userver::formats::json::Value &,
                                                                                            userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const boost::uuids::uuid template_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("template_id"));
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_template_manager.DeleteTemplate(user_id, template_id);
    return userver::formats::json::Value{};
  }
  catch (const ens::user::JwtVerificationException &e) {
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
  catch (const NotificationTemplateNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::templates::AppendTemplateDeleteHandler(userver::components::ComponentList &component_list) {
  component_list.Append<TemplateDeleteHandler>();
}
