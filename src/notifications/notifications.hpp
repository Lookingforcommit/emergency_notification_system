#pragma once

#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "utils/utils.hpp"
#include "schemas/schemas.hpp"
#include "notifications/telegram/telegram_bot.hpp"

namespace ens::notifications {
// Component for notifications management logic
class NotificationsManager : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "notification-manager";
  NotificationsManager(const userver::components::ComponentConfig &config,
                       const userver::components::ComponentContext &component_context) :
      ComponentBase(config, component_context),
      _pg_cluster(
          component_context
              .FindComponent<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
              .GetCluster()),
      _telegram_bot(component_context.FindComponent<ens::notifications::telegram::TelegramNotificationsBot>()) {}

  static userver::yaml_config::Schema GetStaticConfigSchema();
  std::string CreateBatch(const boost::uuids::uuid &user_id);
  std::unique_ptr<schemas::Notification> GetById(const boost::uuids::uuid &user_id,
                                                 const boost::uuids::uuid &notification_id) const;
  std::unique_ptr<schemas::NotificationList> GetAll(const boost::uuids::uuid &user_id) const;
  std::unique_ptr<schemas::NotificationList> GetPending(const boost::uuids::uuid &user_id) const;
  std::unique_ptr<std::vector<std::string>> SendBatch(const boost::uuids::uuid &user_id,
                                                      const boost::uuids::uuid &batch_id);
  void CancelNotification(const boost::uuids::uuid &user_id, const boost::uuids::uuid &notification_id);
 private:
  userver::storages::postgres::ClusterPtr _pg_cluster;
  ens::notifications::telegram::TelegramNotificationsBot &_telegram_bot;
  std::string CreateNotification(const schemas::Notification::Type &type,
                                 const boost::uuids::uuid &batch_id,
                                 const boost::uuids::uuid &recipient_id,
                                 const boost::uuids::uuid &group_id);
};

void AppendNotificationsManager(userver::components::ComponentList &component_list);

class NotificationNotFoundException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Notification does not exist/has already been sent notification_id={}"};
  const std::string _msg;
 public:
  NotificationNotFoundException(const std::string &notification_id) : _msg(fmt::format(this->FORMAT,
                                                                                       notification_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

class NotificationBatchNotFoundException : public std::exception {
 private:
  static constexpr std::string_view FORMAT{"Notifications batch does not exist/has already been sent batch_id={}"};
  const std::string _msg;
 public:
  NotificationBatchNotFoundException(const std::string &draft_id) : _msg(fmt::format(this->FORMAT, draft_id)) {};
  [[nodiscard]] const char *what() const
  noexcept override { return this->_msg.c_str(); };
};

}