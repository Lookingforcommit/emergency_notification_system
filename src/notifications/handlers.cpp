#include "handlers.hpp"

userver::formats::json::Value ens::notifications::NotificationCreateBatchHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                         const userver::formats::json::Value &,
                                                                                                         userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    const std::string batch_id = this->_notification_manager.CreateBatch(user_id);
    userver::formats::json::ValueBuilder vb{batch_id};
    return vb.ExtractValue();
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::notifications::AppendNotificationCreateBatchHandler(userver::components::ComponentList &component_list) {
  component_list.Append<NotificationCreateBatchHandler>();
}

userver::formats::json::Value ens::notifications::NotificationGetByIdHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                     const userver::formats::json::Value &,
                                                                                                     userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    const boost::uuids::uuid
        notification_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("notification_id"));
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::Notification>
        notification = this->_notification_manager.GetById(user_id, notification_id);
    return schemas::Serialize(*notification, userver::formats::serialize::To<userver::formats::json::Value>());
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
  catch (const NotificationNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::notifications::AppendNotificationGetByIdHandler(userver::components::ComponentList &component_list) {
  component_list.Append<NotificationGetByIdHandler>();
}

userver::formats::json::Value ens::notifications::NotificationGetAllHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                    const userver::formats::json::Value &,
                                                                                                    userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::NotificationList> notifications = this->_notification_manager.GetAll(user_id);
    userver::formats::json::ValueBuilder notifications_json(userver::formats::common::Type::kArray);
    for (const auto &item : *notifications) {
      notifications_json.PushBack(schemas::Serialize(item,
                                                     userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return notifications_json.ExtractValue();
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::notifications::AppendNotificationGetAllHandler(userver::components::ComponentList &component_list) {
  component_list.Append<NotificationGetAllHandler>();
}

userver::formats::json::Value ens::notifications::NotificationGetPendingHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                        const userver::formats::json::Value &,
                                                                                                        userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<schemas::NotificationList> notifications = this->_notification_manager.GetPending(user_id);
    userver::formats::json::ValueBuilder notifications_json(userver::formats::common::Type::kArray);
    for (const auto &item : *notifications) {
      notifications_json.PushBack(schemas::Serialize(item,
                                                     userver::formats::serialize::To<userver::formats::json::Value>()));
    }
    return notifications_json.ExtractValue();
  }
  catch (const ens::auth::GenericJWTException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kUnauthorized,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::notifications::AppendNotificationGetPendingHandler(userver::components::ComponentList &component_list) {
  component_list.Append<NotificationGetPendingHandler>();
}

userver::formats::json::Value ens::notifications::NotificationSendBatchHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                       const userver::formats::json::Value &,
                                                                                                       userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    const boost::uuids::uuid batch_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("batch_id"));
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    std::unique_ptr<std::vector<std::string>>
        batch = this->_notification_manager.SendBatch(user_id, batch_id);
    userver::formats::json::ValueBuilder ids_json(userver::formats::common::Type::kArray);
    for (const auto &notif_id : *batch) {
      ids_json.PushBack(notif_id);
    }
    return ids_json.ExtractValue();
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
  catch (const NotificationBatchNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::notifications::AppendNotificationSendBatchHandler(userver::components::ComponentList &component_list) {
  component_list.Append<NotificationSendBatchHandler>();
}

userver::formats::json::Value ens::notifications::NotificationCancelNotificationHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                                                                                const userver::formats::json::Value &,
                                                                                                                userver::server::request::RequestContext &) const {
  const std::string &access_token = request.GetHeader("Authorization");
  try {
    const boost::uuids::uuid notification_id = boost::lexical_cast<boost::uuids::uuid>(request.GetArg("notification_id"));
    const boost::uuids::uuid user_id = _jwt_verif_manager.VerifyJWT(access_token);
    this->_notification_manager.CancelNotification(user_id, notification_id);
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
  catch (const NotificationNotFoundException &e) {
    throw userver::server::handlers::CustomHandlerException{
        userver::server::handlers::HandlerErrorCode::kResourceNotFound,
        userver::server::handlers::InternalMessage{e.what()},
        userver::server::handlers::ExternalBody{e.what()}
    };
  }
}

void ens::notifications::AppendNotificationCancelNotificationHandler(userver::components::ComponentList &component_list) {
  component_list.Append<NotificationCancelNotificationHandler>();
}
