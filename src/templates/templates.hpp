#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "utils/utils.hpp"
#include "schemas/schemas.hpp"

namespace ens::templates {
// Component for templates management logic
class TemplateManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "template-manager";
  TemplateManager(const userver::components::ComponentConfig &config,
                  const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _pg_cluster(
          component_context
              .FindComponent<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
              .GetCluster()) {}
  static userver::yaml_config::Schema GetStaticConfigSchema();
  std::unique_ptr<schemas::NotificationTemplateDraft> Create(const boost::uuids::uuid &user_id,
                                                             const schemas::NotificationTemplateWithoutId &data);
  std::unique_ptr<schemas::NotificationTemplateWithId> GetById(const boost::uuids::uuid &user_id,
                                                               const boost::uuids::uuid &template_id) const;
  std::unique_ptr<schemas::NotificationTemplateWithIdList> GetAll(const boost::uuids::uuid &user_id) const;
  std::unique_ptr<schemas::NotificationTemplateWithId> ConfirmCreation(const boost::uuids::uuid &user_id,
                                                                       const boost::uuids::uuid &draft_id);
  std::unique_ptr<schemas::NotificationTemplateWithId> ModifyTemplate(const boost::uuids::uuid &user_id,
                                                                      const boost::uuids::uuid &template_id,
                                                                      const schemas::NotificationTemplateWithoutId &data);
  void DeleteTemplate(const boost::uuids::uuid &user_id, const boost::uuids::uuid &template_id);
 private:
  userver::storages::postgres::ClusterPtr _pg_cluster;
};

void AppendTemplateManager(userver::components::ComponentList &component_list);

class NotificationTemplateNotFoundException : public std::exception {
 private:
  const std::string FORMAT{"Recipient does not exist template_id={}"};
  const std::string _msg;
 public:
  NotificationTemplateNotFoundException(const std::string &template_id) : _msg(fmt::format(this->FORMAT,
                                                                                           template_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class DraftNotFoundException : public std::exception {
 private:
  const std::string FORMAT{"Draft does not exist draft_id={}"};
  const std::string _msg;
 public:
  DraftNotFoundException(const std::string &draft_id) : _msg(fmt::format(this->FORMAT, draft_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};
}