#pragma once

#include <fmt/format.h>
#include <memory>

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "user/auth.hpp"
#include "schemas/schemas.hpp"
#include "utils/utils.hpp"

namespace ens::recipients {

// Component for recipients management logic
class RecipientManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "recipient-manager";
  RecipientManager(const userver::components::ComponentConfig &config,
                   const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _pg_cluster(
          component_context
              .FindComponent<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
              .GetCluster()) {}
  static userver::yaml_config::Schema GetStaticConfigSchema();
  std::unique_ptr<schemas::RecipientDraft> Create(const boost::uuids::uuid &user_id, const schemas::RecipientWithoutId &data);
  std::unique_ptr<schemas::RecipientWithId> GetById(const boost::uuids::uuid &user_id, const std::string &recipient_id) const;
  std::unique_ptr<schemas::RecipientWithIdList> GetAll(const boost::uuids::uuid &user_id) const;
  std::unique_ptr<schemas::RecipientWithId> ConfirmCreation(const boost::uuids::uuid &user_id, const std::string &draft_id);
  std::unique_ptr<schemas::RecipientWithId> ModifyRecipient(const boost::uuids::uuid &user_id, const std::string &recipient_id,
                                                            const schemas::RecipientWithoutId &data);
  void DeleteRecipient(const boost::uuids::uuid &user_id, const std::string &recipient_id);
 private:
  userver::storages::postgres::ClusterPtr _pg_cluster;
};

void AppendRecipientManager(userver::components::ComponentList &component_list);

class RecipientNotFoundException : public std::exception {
 private:
  const std::string FORMAT{"Recipient does not exist recipient_id={}"};
  const std::string _msg;
 public:
  RecipientNotFoundException(const std::string &recipient_id) : _msg(fmt::format(this->FORMAT, recipient_id)) {};
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

}  // namespace ens::recipients

