#pragma once

#include <memory>
#include <string>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>

#include "notifications/notifications.hpp"
#include "user/auth.hpp"

namespace ens::notifications {
class NotificationJsonHandlerBase : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  NotificationJsonHandlerBase(const userver::components::ComponentConfig &config,
                              const userver::components::ComponentContext &context)
      : HttpHandlerJsonBase(config, context),
        _notification_manager(context.FindComponent<NotificationsManager>()),
        _jwt_verif_manager(context.FindComponent<ens::auth::JWTManager>()) {}
 protected:
  NotificationsManager &_notification_manager;
  ens::auth::JWTManager &_jwt_verif_manager;
};

class NotificationCreateBatchHandler : public NotificationJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-notifications-createBatch";
  using NotificationJsonHandlerBase::NotificationJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendNotificationCreateBatchHandler(userver::components::ComponentList &component_list);

class NotificationGetByIdHandler : public NotificationJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-notifications-getById";
  using NotificationJsonHandlerBase::NotificationJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendNotificationGetByIdHandler(userver::components::ComponentList &component_list);

class NotificationGetAllHandler : public NotificationJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-notifications-getAll";
  using NotificationJsonHandlerBase::NotificationJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendNotificationGetAllHandler(userver::components::ComponentList &component_list);

class NotificationGetPendingHandler : public NotificationJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-notifications-getPending";
  using NotificationJsonHandlerBase::NotificationJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendNotificationGetPendingHandler(userver::components::ComponentList &component_list);

class NotificationSendBatchHandler : public NotificationJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-notifications-sendBatch";
  using NotificationJsonHandlerBase::NotificationJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendNotificationSendBatchHandler(userver::components::ComponentList &component_list);

class NotificationCancelNotificationHandler : public NotificationJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-notifications-cancelNotification";
  using NotificationJsonHandlerBase::NotificationJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendNotificationCancelNotificationHandler(userver::components::ComponentList &component_list);

}
