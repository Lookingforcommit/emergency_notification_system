#pragma once

#include <memory>
#include <string>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>

#include "templates/templates.hpp"
#include "user/auth.hpp"

namespace ens::templates {
class TemplateJsonHandlerBase : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  TemplateJsonHandlerBase(const userver::components::ComponentConfig &config,
                          const userver::components::ComponentContext &context)
      : HttpHandlerJsonBase(config, context),
        _template_manager(context.FindComponent<TemplateManager>()),
        _jwt_verif_manager(context.FindComponent<ens::auth::JWTManager>()) {}
 protected:
  TemplateManager &_template_manager;
  ens::auth::JWTManager &_jwt_verif_manager;
};

class TemplateCreateHandler : public TemplateJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-templates-create";
  using TemplateJsonHandlerBase::TemplateJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendTemplateCreateHandler(userver::components::ComponentList &component_list);

class TemplateGetByIdHandler : public TemplateJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-templates-getById";
  using TemplateJsonHandlerBase::TemplateJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendTemplateGetByIdHandler(userver::components::ComponentList &component_list);

class TemplateGetAllHandler : public TemplateJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-templates-getAll";
  using TemplateJsonHandlerBase::TemplateJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendTemplateGetAllHandler(userver::components::ComponentList &component_list);

class TemplateConfirmCreationHandler : public TemplateJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-templates-confirmCreation";
  using TemplateJsonHandlerBase::TemplateJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendTemplateConfirmCreationHandler(userver::components::ComponentList &component_list);

class TemplateModifyHandler : public TemplateJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-templates-modifyTemplate";
  using TemplateJsonHandlerBase::TemplateJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &request_json,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendTemplateModifyHandler(userver::components::ComponentList &component_list);

class TemplateDeleteHandler : public TemplateJsonHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-templates-deleteTemplate";
  using TemplateJsonHandlerBase::TemplateJsonHandlerBase;
  userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest &request,
                                                       const userver::formats::json::Value &,
                                                       userver::server::request::RequestContext &) const override;
};

void AppendTemplateDeleteHandler(userver::components::ComponentList &component_list);

}
