#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "utils/utils.hpp"
#include "schemas/schemas.hpp"

namespace ens::groups {
// Component for groups management logic
class GroupManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "group-manager";
  GroupManager(const userver::components::ComponentConfig &config,
               const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _pg_cluster(
          component_context
              .FindComponent<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
              .GetCluster()) {}
  static userver::yaml_config::Schema GetStaticConfigSchema();
  std::unique_ptr<schemas::RecipientGroupDraft> Create(const boost::uuids::uuid &user_id,
                                                       const schemas::RecipientGroupWithoutId &data);
  std::unique_ptr<schemas::RecipientGroupWithId> GetById(const boost::uuids::uuid &user_id,
                                                         const boost::uuids::uuid &group_id) const;
  std::unique_ptr<schemas::RecipientWithIdList> GetRecipients(const boost::uuids::uuid &user_id,
                                                              const boost::uuids::uuid &group_id) const;
  std::unique_ptr<schemas::RecipientGroupWithIdList> GetActive(const boost::uuids::uuid &user_id) const;
  std::unique_ptr<schemas::RecipientGroupWithIdList> GetAll(const boost::uuids::uuid &user_id) const;
  std::unique_ptr<schemas::RecipientGroupWithId> ConfirmCreation(const boost::uuids::uuid &user_id,
                                                                 const boost::uuids::uuid &draft_id);
  std::unique_ptr<schemas::RecipientGroupWithId> ModifyGroup(const boost::uuids::uuid &user_id,
                                                             const boost::uuids::uuid &group_id,
                                                             const schemas::RecipientGroupWithoutId &data);
  void AddRecipient(const boost::uuids::uuid &user_id,
                    const boost::uuids::uuid &group_id,
                    const boost::uuids::uuid &recipient_id);
  void DeleteGroup(const boost::uuids::uuid &user_id, const boost::uuids::uuid &group_id);
  void DeleteRecipient(const boost::uuids::uuid &user_id,
                       const boost::uuids::uuid &group_id,
                       const boost::uuids::uuid &recipient_id);
 private:
  userver::storages::postgres::ClusterPtr _pg_cluster;
};

void AppendGroupManager(userver::components::ComponentList &component_list);

class IncorrectNotificationTemplateIdException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Notification template does not exist template_id={}"};
  const std::string _msg;
 public:
  IncorrectNotificationTemplateIdException(const std::string &template_id) : _msg(fmt::format(this->FORMAT,
                                                                                              template_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class RecipientGroupNotFoundException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Group does not exist group_id={}"};
  const std::string _msg;
 public:
  RecipientGroupNotFoundException(const std::string &group_id) : _msg(fmt::format(this->FORMAT,
                                                                                  group_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class DraftNotFoundException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Draft does not exist draft_id={}"};
  const std::string _msg;
 public:
  DraftNotFoundException(const std::string &draft_id) : _msg(fmt::format(this->FORMAT, draft_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class RecipientNotFoundException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Recipient does not exist recipient_id={}"};
  const std::string _msg;
 public:
  RecipientNotFoundException(const std::string &recipient_id) : _msg(fmt::format(this->FORMAT, recipient_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class RecipientAlreadyAddedException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Recipient already added to the group recipient_id={}, group_id={}"};
  const std::string _msg;
 public:
  RecipientAlreadyAddedException(const std::string &recipient_id, const std::string &group_id)
      : _msg(fmt::format(this->FORMAT, recipient_id, group_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class RecipientNotAddedException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Recipient not added to the group recipient_id={}, group_id={}"};
  const std::string _msg;
 public:
  RecipientNotAddedException(const std::string &recipient_id, const std::string &group_id)
      : _msg(fmt::format(this->FORMAT, recipient_id, group_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};
}