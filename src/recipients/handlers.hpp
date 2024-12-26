#pragma once

#include <memory>
#include <string>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>

#include "recipients/recipients.hpp"
#include "user/auth.hpp"

namespace ens::recipients {
class RecipientJsonHandlerBase : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  RecipientJsonHandlerBase(const userver::components::ComponentConfig &config,
                           const userver::components::ComponentContext &context)
      : HttpHandlerJsonBase(config, context),
        _recipient_manager(context.FindComponent<RecipientManager>()),
        _jwt_verif_manager(context.FindComponent<ens::user::JwtManager>()) {}
 protected:
  RecipientManager &_recipient_manager;
  ens::user::JwtManager &_jwt_verif_manager;
};

class RecipientCreateHandler : public RecipientJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-recipients-create";
  using RecipientJsonHandlerBase::RecipientJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendRecipientCreateHandler(userver::components::ComponentList &component_list);

class RecipientGetByIdHandler : public RecipientJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-recipients-getById";
  using RecipientJsonHandlerBase::RecipientJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendRecipientGetByIdHandler(userver::components::ComponentList &component_list);

class RecipientGetAllHandler : public RecipientJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-recipients-getAll";
  using RecipientJsonHandlerBase::RecipientJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendRecipientGetAllHandler(userver::components::ComponentList &component_list);

class RecipientConfirmCreationHandler : public RecipientJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-recipients-confirmCreation";
  using RecipientJsonHandlerBase::RecipientJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendRecipientConfirmCreationHandler(userver::components::ComponentList &component_list);

class RecipientModifyHandler : public RecipientJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-recipients-modifyRecipient";
  using RecipientJsonHandlerBase::RecipientJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendRecipientModifyHandler(userver::components::ComponentList &component_list);

class RecipientDeleteHandler : public RecipientJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-recipients-deleteRecipient";
  using RecipientJsonHandlerBase::RecipientJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendRecipientDeleteHandler(userver::components::ComponentList &component_list);

}
