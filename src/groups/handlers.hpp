#pragma once

#include <memory>
#include <string>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>

#include "groups/groups.hpp"
#include "user/auth.hpp"

namespace ens::groups {
class GroupJsonHandlerBase : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  GroupJsonHandlerBase(const userver::components::ComponentConfig &config,
                       const userver::components::ComponentContext &context)
      : HttpHandlerJsonBase(config, context),
        _group_manager(context.FindComponent<GroupManager>()),
        _jwt_verif_manager(context.FindComponent<ens::user::JwtManager>()) {}
 protected:
  GroupManager &_group_manager;
  ens::user::JwtManager &_jwt_verif_manager;
};

class GroupCreateHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-create";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupCreateHandler(userver::components::ComponentList &component_list);

class GroupGetByIdHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-getById";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupGetByIdHandler(userver::components::ComponentList &component_list);

class GroupGetRecipientsHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-getRecipients";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupGetRecipientsHandler(userver::components::ComponentList &component_list);

class GroupGetActiveHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-getActive";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupGetActiveHandler(userver::components::ComponentList &component_list);

class GroupGetAllHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-getAll";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupGetAllHandler(userver::components::ComponentList &component_list);

class GroupConfirmCreationHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-confirmCreation";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupConfirmCreationHandler(userver::components::ComponentList &component_list);

class GroupModifyGroupHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-modifyGroup";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupModifyGroupHandler(userver::components::ComponentList &component_list);

class GroupAddRecipientHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-addRecipient";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupAddRecipientHandler(userver::components::ComponentList &component_list);

class GroupDeleteRecipientHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-deleteRecipient";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupDeleteRecipientHandler(userver::components::ComponentList &component_list);

class GroupDeleteGroupHandler : public GroupJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-groups-deleteGroup";
  using GroupJsonHandlerBase::GroupJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendGroupDeleteGroupHandler(userver::components::ComponentList &component_list);

}