#include "notifications.hpp"

#include <memory>

#include <userver/yaml_config/merge_schemas.hpp>

#include "schemas/schemas.hpp"

userver::yaml_config::Schema ens::notifications::NotificationsManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: Component for notifications management logic
    additionalProperties: false
    properties: {}
  )");
}

std::string ens::notifications::NotificationsManager::CreateBatch(const boost::uuids::uuid &user_id) {
  const userver::storages::postgres::Query create_batch_query{
      "INSERT INTO ens_schema.notifications_batch "
      "(batch_id, master_id, sent) "
      "VALUES ($1, $2, false)"
  };
  boost::uuids::uuid batch_id = userver::utils::generators::GenerateBoostUuidV7();
  userver::storages::postgres::Transaction insert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet insert_res = insert_transaction.Execute(create_batch_query,
                                                                                 batch_id,
                                                                                 user_id);
  insert_transaction.Commit();
  return boost::uuids::to_string(batch_id);
}

std::unique_ptr<schemas::Notification> ens::notifications::NotificationsManager::GetById(const boost::uuids::uuid &user_id,
                                                                                         const boost::uuids::uuid &notification_id) const {
  const userver::storages::postgres::Query notification_info_query{
      "SELECT notification_id, batch_id, recipient_id, group_id, type, creation_timestamp, completion_timestamp "
      "FROM ens_schema.notification INNER JOIN ens_schema.notifications_batch USING(batch_id) "
      "WHERE master_id = $1 AND notification_id = $2",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        notification_info_query,
                                        user_id,
                                        notification_id);
  if (select_res.IsEmpty()) {
    throw NotificationNotFoundException{boost::uuids::to_string(notification_id)};
  }
  userver::storages::postgres::Row notification_row = select_res[0];
  schemas::Notification notification_data{boost::uuids::to_string(notification_id),
                                          boost::uuids::to_string(notification_row["batch_id"].As<boost::uuids::uuid>()),
                                          boost::uuids::to_string(notification_row["recipient_id"].As<boost::uuids::uuid>()),
                                          boost::uuids::to_string(notification_row["group_id"].As<boost::uuids::uuid>()),
                                          notification_row["message_type"].As<schemas::Notification::Type>(),
                                          notification_row["creation_timestamp"].As<std::string>(),
                                          notification_row["completion_timestamp"].As<std::optional<std::string>>()
  };
  return std::make_unique<schemas::Notification>(notification_data);
}

std::unique_ptr<schemas::NotificationList> ens::notifications::NotificationsManager::GetAll(const boost::uuids::uuid &user_id) const {
  const userver::storages::postgres::Query notification_info_query{
      "SELECT notification_id, batch_id, recipient_id, group_id, type, creation_timestamp, completion_timestamp "
      "FROM ens_schema.notification INNER JOIN ens_schema.notifications_batch USING(batch_id) "
      "WHERE master_id = $1",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        notification_info_query,
                                        user_id);
  schemas::NotificationList notifs_data;
  for (auto row : select_res) {
    userver::storages::postgres::Row notification_row = select_res[0];
    schemas::Notification
        notification_data{boost::uuids::to_string(notification_row["notification_id"].As<boost::uuids::uuid>()),
                          boost::uuids::to_string(notification_row["batch_id"].As<boost::uuids::uuid>()),
                          boost::uuids::to_string(notification_row["recipient_id"].As<boost::uuids::uuid>()),
                          boost::uuids::to_string(notification_row["group_id"].As<boost::uuids::uuid>()),
                          notification_row["message_type"].As<schemas::Notification::Type>(),
                          notification_row["creation_timestamp"].As<std::string>(),
                          notification_row["completion_timestamp"].As<std::optional<std::string>>()
    };
    notifs_data.emplace_back(notification_data);
  }
  return std::make_unique<schemas::NotificationList>(notifs_data);
}

std::unique_ptr<schemas::NotificationList> ens::notifications::NotificationsManager::GetPending(const boost::uuids::uuid &user_id) const {
  const userver::storages::postgres::Query notification_info_query{
      "SELECT notification_id, batch_id, recipient_id, group_id, type, creation_timestamp, completion_timestamp "
      "FROM ens_schema.notification INNER JOIN ens_schema.notifications_batch USING(batch_id) "
      "WHERE master_id = $1 AND completion_timestamp IS NULL",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        notification_info_query,
                                        user_id);
  schemas::NotificationList notifs_data;
  for (auto row : select_res) {
    userver::storages::postgres::Row notification_row = select_res[0];
    schemas::Notification
        notification_data{boost::uuids::to_string(notification_row["notification_id"].As<boost::uuids::uuid>()),
                          boost::uuids::to_string(notification_row["batch_id"].As<boost::uuids::uuid>()),
                          boost::uuids::to_string(notification_row["recipient_id"].As<boost::uuids::uuid>()),
                          boost::uuids::to_string(notification_row["group_id"].As<boost::uuids::uuid>()),
                          notification_row["message_type"].As<schemas::Notification::Type>(),
                          notification_row["creation_timestamp"].As<std::string>(),
                          notification_row["completion_timestamp"].As<std::optional<std::string>>()
    };
    notifs_data.emplace_back(notification_data);
  }
  return std::make_unique<schemas::NotificationList>(notifs_data);
}

// TODO: Add functionality to keep track of notifications status
std::string ens::notifications::NotificationsManager::CreateNotification(const schemas::Notification::Type &type,
                                                                         const boost::uuids::uuid &batch_id,
                                                                         const boost::uuids::uuid &recipient_id,
                                                                         const boost::uuids::uuid &group_id) {
  const userver::storages::postgres::Query create_notification_query{
      "INSERT INTO ens_schema.notification "
      "(type, creation_timestamp, notification_id, batch_id, recipient_id, group_id) "
      "VALUES ($1, $2, $3, $4, $5, $6)"
  };
  const boost::uuids::uuid notification_id = userver::utils::generators::GenerateBoostUuidV7();
  std::time_t creation_timestamp = userver::utils::datetime::Timestamp();
  userver::storages::postgres::Transaction insert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet insert_res = insert_transaction.Execute(create_notification_query,
                                                                                 type,
                                                                                 creation_timestamp,
                                                                                 notification_id,
                                                                                 batch_id,
                                                                                 recipient_id,
                                                                                 group_id);
  insert_transaction.Commit();
  return boost::uuids::to_string(notification_id);
}

std::unique_ptr<std::vector<std::string>> ens::notifications::NotificationsManager::SendBatch(const boost::uuids::uuid &user_id,
                                                                                              const boost::uuids::uuid &batch_id) {
  const userver::storages::postgres::Query batch_exists_query{
      "SELECT EXISTS ( "
      "SELECT 1 "
      "FROM ens_schema.notifications_batch "
      "WHERE master_id = $1 AND batch_id = $2 AND NOT sent)"
  };
  const userver::storages::postgres::Query set_batch_sent_query{
      "UPDATE ens_schema.notifications_batch "
      "SET sent = true "
      "WHERE master_id = $1 AND batch_id = $2"
  };
  const userver::storages::postgres::Query info_query{
      "SELECT recipient_group.recipient_group_id, recipient.recipient_id, recipient.email, recipient.phone_number, recipient.telegram_id, telegram_contact.active, notification_template.message_text "
      "FROM ens_schema.recipient_group "
      "INNER JOIN ens_schema.notification_template ON recipient_group.template_id = notification_template.notification_template_id "  // Inner join elliminates groups without template
      "INNER JOIN ens_schema.recipient_recipient_group ON recipient_group.recipient_group_id = recipient_recipient_group.recipient_group_id "
      "INNER JOIN ens_schema.recipient ON recipient_recipient_group.recipient_id = recipient.recipient_id "
      "LEFT JOIN ens_schema.telegram_contact ON recipient.telegram_id = telegram_contact.user_id "
      "WHERE recipient_group.master_id = $1 AND recipient_group.active"
  };
  userver::storages::postgres::ResultSet
      batch_exists_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                              batch_exists_query,
                                              user_id,
                                              batch_id);
  if (not batch_exists_res.AsSingleRow<bool>()) {
    throw NotificationBatchNotFoundException{boost::uuids::to_string(batch_id)};
  }
  userver::storages::postgres::Transaction set_batch_sent_tr = _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  set_batch_sent_tr.Execute(set_batch_sent_query,
                            user_id,
                            batch_id);
  set_batch_sent_tr.Commit();
  userver::storages::postgres::ResultSet
      info_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                      info_query,
                                      user_id);
  std::optional<bool> telegram_active;
  int64_t telegram_id;
  std::optional<std::string> template_text;
  boost::uuids::uuid recipient_id;
  boost::uuids::uuid group_id;
  std::vector<std::string> ids_vector;
  for (auto row : info_res) {
    telegram_active = row["active"].As<std::optional<bool>>();
    if (not telegram_active.has_value() or not telegram_active.value()) {
      continue;
    }
    template_text = row["message_text"].As<std::optional<std::string>>();
    if (not template_text.has_value()) {
      continue;
    }
    telegram_id = row["telegram_id"].As<int64_t>();
    recipient_id = row["recipient_id"].As<boost::uuids::uuid>();
    group_id = row["recipient_group_id"].As<boost::uuids::uuid>();
    ids_vector.push_back(CreateNotification(schemas::Notification::Type::kTelegram, batch_id, recipient_id, group_id));
    this->_telegram_bot.SendMessage(telegram_id, template_text.value());
  }
  return std::make_unique<std::vector<std::string>>(ids_vector);
}

void ens::notifications::NotificationsManager::CancelNotification(const boost::uuids::uuid &user_id,
                                                                  const boost::uuids::uuid &notification_id) {
  const userver::storages::postgres::Query deletion_query{
      "DELETE FROM ens_schema.notification "
      "USING ens_schema.notifications_batch "
      "WHERE notification.batch_id = notifications_batch.batch_id "
      "AND notification.notification_id = $2 "
      "AND notifications_batch.master_id = $1 "
      "AND notification.completion_timestamp IS NULL",
  };
  userver::storages::postgres::Transaction deletion_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      deletion_res = deletion_transaction.Execute(deletion_query, user_id, notification_id);
  if (not deletion_res.RowsAffected()) {
    deletion_transaction.Rollback();
    throw NotificationNotFoundException{boost::uuids::to_string(notification_id)};
  }
  deletion_transaction.Commit();
}

void ens::notifications::AppendNotificationsManager(userver::components::ComponentList &component_list) {
  component_list.Append<NotificationsManager>();
}
