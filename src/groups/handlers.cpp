#include "handlers.hpp"

#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_error.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "schemas/schemas.hpp"
#include "user/auth.hpp"

userver::formats::json::Value ens::groups::GroupCreateHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                      const userver::formats::json::Value &request_json,
                                                                                      userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    schemas::RecipientGroupWithoutId user_data = request_json.As<schemas::RecipientGroupWithoutId>();
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientGroupDraft>
        created_data = this->_group_manager.Create(user_id, user_data);
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

void ens::groups::AppendGroupCreateHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupCreateHandler>();
}

userver::formats::json::Value ens::groups::GroupGetByIdHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                       const userver::formats::json::Value &,
                                                                                       userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string group_id = request.GetArg("group_id");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientGroupWithId>
        recipient_group = this->_group_manager.GetById(user_id, group_id);
    return schemas::Serialize(*recipient_group, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientGroupNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::groups::AppendGroupGetByIdHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupGetByIdHandler>();
}

userver::formats::json::Value ens::groups::GroupGetRecipientsHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                             const userver::formats::json::Value &,
                                                                                             userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string &group_id = request.GetArg("group_id");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientWithIdList> recipients = this->_group_manager.GetRecipients(user_id, group_id);
    userver::formats::json::ValueBuilder groups_json(userver::formats::common::Type::kArray);
    for (const auto &item : *recipients) {
      groups_json.PushBack(schemas::Serialize(item,
                                              userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return groups_json.ExtractValue();
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientGroupNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::groups::AppendGroupGetRecipientsHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupGetRecipientsHandler>();
}

userver::formats::json::Value ens::groups::GroupGetActiveHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                         const userver::formats::json::Value &,
                                                                                         userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientGroupWithIdList> groups = this->_group_manager.GetActive(user_id);
    userver::formats::json::ValueBuilder groups_json(userver::formats::common::Type::kArray);
    for (const auto &item : *groups) {
      groups_json.PushBack(schemas::Serialize(item,
                                              userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return groups_json.ExtractValue();
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::groups::AppendGroupGetActiveHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupGetActiveHandler>();
}

userver::formats::json::Value ens::groups::GroupGetAllHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                      const userver::formats::json::Value &,
                                                                                      userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientGroupWithIdList> groups = this->_group_manager.GetAll(user_id);
    userver::formats::json::ValueBuilder groups_json(userver::formats::common::Type::kArray);
    for (const auto &item : *groups) {
      groups_json.PushBack(schemas::Serialize(item,
                                              userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return groups_json.ExtractValue();
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::groups::AppendGroupGetAllHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupGetAllHandler>();
}

userver::formats::json::Value ens::groups::GroupConfirmCreationHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                               const userver::formats::json::Value &,
                                                                                               userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string draft_id = request.GetArg("draft_id");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientGroupWithId>
        recipient_group = this->_group_manager.ConfirmCreation(user_id, draft_id);
    return schemas::Serialize(*recipient_group, userver::formats::serialize::To<userver::formats::json::Value>());
  }
  catch (const ens::user::JwtVerificationException &e) {
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

void ens::groups::AppendGroupConfirmCreationHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupConfirmCreationHandler>();
}

userver::formats::json::Value ens::groups::GroupModifyGroupHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                           const userver::formats::json::Value &request_json,
                                                                                           userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string group_id = request.GetArg("group_id");
  try {
    schemas::RecipientGroupWithoutId user_data = request_json.As<schemas::RecipientGroupWithoutId>();
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    std::unique_ptr<schemas::RecipientGroupWithId> modified_data = this->_group_manager.ModifyGroup(user_id,
                                                                                                    group_id,
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
  catch (const RecipientGroupNotFoundException &e) {
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

void ens::groups::AppendGroupModifyGroupHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupModifyGroupHandler>();
}

userver::formats::json::Value ens::groups::GroupAddRecipientHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                            const userver::formats::json::Value &,
                                                                                            userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string group_id = request.GetArg("group_id");
  const std::string recipient_id = request.GetArg("recipient_id");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_group_manager.AddRecipient(user_id, group_id, recipient_id);
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientGroupNotFoundException &e) {
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
  catch (const RecipientAlreadyAddedException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kConflictState,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  return userver::formats::json::Value{};
}

void ens::groups::AppendGroupAddRecipientHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupAddRecipientHandler>();
}

userver::formats::json::Value ens::groups::GroupDeleteRecipientHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                               const userver::formats::json::Value &,
                                                                                               userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string group_id = request.GetArg("group_id");
  const std::string recipient_id = request.GetArg("recipient_id");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_group_manager.DeleteRecipient(user_id, group_id, recipient_id);
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientGroupNotFoundException &e) {
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
  return userver::formats::json::Value{};
}

void ens::groups::AppendGroupDeleteRecipientHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupDeleteRecipientHandler>();
}

userver::formats::json::Value ens::groups::GroupDeleteGroupHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                           const userver::formats::json::Value &,
                                                                                           userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  const std::string group_id = request.GetArg("group_id");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJwt(access_token);
    this->_group_manager.DeleteGroup(user_id, group_id);
  }
  catch (const ens::user::JwtVerificationException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  catch (const RecipientGroupNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
  return userver::formats::json::Value{};
}

void ens::groups::AppendGroupDeleteGroupHandler(userver::components::ComponentList &component_list) {
  component_list.Append<GroupDeleteGroupHandler>();
}
