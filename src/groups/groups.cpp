#include "groups.hpp"

#include <memory>

#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/utils/boost_uuid7.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>

#include "schemas/schemas.hpp"

userver::yaml_config::Schema ens::groups::GroupManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: Component for groups management logic
    additionalProperties: false
    properties: {}
  )");
}

std::unique_ptr<schemas::RecipientGroupDraft> ens::groups::GroupManager::Create(const boost::uuids::uuid &user_id,
                                                                                const schemas::RecipientGroupWithoutId &data) {
  const userver::storages::postgres::Query create_group_query{
      "INSERT INTO ens_schema.recipient_group_draft "
      "(recipient_group_draft_id, master_id, template_id, name, active) "
      "VALUES ($1, $2, $3, $4, $5)"
  };
  boost::uuids::uuid group_draft_id = userver::utils::generators::GenerateBoostUuidV7();
  userver::storages::postgres::Transaction insert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  try {
    userver::storages::postgres::ResultSet insert_res = insert_transaction.Execute(create_group_query,
                                                                                   group_draft_id,
                                                                                   user_id,
                                                                                   ens::utils::optional_str_to_uuid(data.notification_template_id),
                                                                                   data.name,
                                                                                   data.active);
    insert_transaction.Commit();
  }
  catch (const boost::bad_lexical_cast &e) { // means that template_id is not null but non-uuid
    throw IncorrectNotificationTemplateIdException{data.notification_template_id.value()};
  }
  catch (const userver::storages::postgres::ForeignKeyViolation &e) { // means that template_id is not null but does not exist
    throw IncorrectNotificationTemplateIdException{data.notification_template_id.value()};
  }
  schemas::RecipientGroupDraft created_draft{boost::uuids::to_string(group_draft_id),
                                             boost::uuids::to_string(user_id),
                                             data.name,
                                             data.notification_template_id,
                                             data.active};
  return std::make_unique<schemas::RecipientGroupDraft>(created_draft);
}

std::unique_ptr<schemas::RecipientGroupWithId> ens::groups::GroupManager::GetById(const boost::uuids::uuid &user_id,
                                                                                  const boost::uuids::uuid &group_id) const {
  const userver::storages::postgres::Query group_info_query{
      "SELECT recipient_group_id, master_id, template_id, name, active "
      "FROM ens_schema.recipient_group "
      "WHERE master_id = $1 AND recipient_group_id = $2",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        group_info_query,
                                        user_id,
                                        group_id);
  if (select_res.IsEmpty()) {
    throw RecipientGroupNotFoundException{boost::uuids::to_string(group_id)};
  }
  userver::storages::postgres::Row group_row = select_res[0];
  schemas::RecipientGroupWithId group_data{boost::uuids::to_string(group_id),
                                           boost::uuids::to_string(user_id),
                                           group_row["name"].As<std::string>(),
                                           group_row["template_id"].As<std::optional<std::string>>(),
                                           group_row["active"].As<bool>()
  };
  return std::make_unique<schemas::RecipientGroupWithId>(group_data);
}

std::unique_ptr<schemas::RecipientWithIdList> ens::groups::GroupManager::GetRecipients(const boost::uuids::uuid &user_id,
                                                                                       const boost::uuids::uuid &group_id) const {
  const userver::storages::postgres::Query group_exists_query{
      "SELECT EXISTS ( "
      "SELECT 1 "
      "FROM ens_schema.recipient_group "
      "WHERE master_id = $1 AND recipient_group_id = $2)"
  };
  const userver::storages::postgres::Query recipients_query{
      "SELECT recipient.recipient_id, recipient.master_id, recipient.name, recipient.email, recipient.phone_number, recipient.telegram_id "
      "FROM ens_schema.recipient_group "
      "LEFT JOIN ens_schema.recipient_recipient_group "
      "ON ens_schema.recipient_group.recipient_group_id = ens_schema.recipient_recipient_group.recipient_group_id "
      "LEFT JOIN ens_schema.recipient "
      "ON ens_schema.recipient.recipient_id = ens_schema.recipient_recipient_group.recipient_id "
      "WHERE recipient_group.master_id = $1 AND recipient_group.recipient_group_id = $2"
  };
  userver::storages::postgres::ResultSet
      group_exists_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                              group_exists_query,
                                              user_id,
                                              group_id);
  if (not group_exists_res.AsSingleRow<bool>()) {
    throw RecipientGroupNotFoundException{boost::uuids::to_string(group_id)};
  }
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        recipients_query,
                                        user_id,
                                        group_id);
  schemas::RecipientWithIdList recipients_data;
  for (auto row : select_res) {
    recipients_data.emplace_back(boost::uuids::to_string(row["recipient_id"].As<boost::uuids::uuid>()),
                                 boost::uuids::to_string(user_id),
                                 row["name"].As<std::string>(),
                                 row["email"].As<std::optional<std::string>>(),
                                 row["phone_number"].As<std::optional<std::string>>(),
                                 row["telegram_id"].As<std::optional<int64_t>>()
    );
  }
  return std::make_unique<schemas::RecipientWithIdList>(recipients_data);
}

std::unique_ptr<schemas::RecipientGroupWithIdList> ens::groups::GroupManager::GetActive(const boost::uuids::uuid &user_id) const {
  const userver::storages::postgres::Query group_info_query{
      "SELECT recipient_group_id, master_id, template_id, name, active "
      "FROM ens_schema.recipient_group "
      "WHERE master_id = $1 AND active = true",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        group_info_query,
                                        user_id);
  schemas::RecipientGroupWithIdList groups_data;
  for (auto row : select_res) {
    groups_data.emplace_back(boost::uuids::to_string(row["recipient_group_id"].As<boost::uuids::uuid>()),
                             boost::uuids::to_string(user_id),
                             row["name"].As<std::string>(),
                             ens::utils::optional_uuid_to_str(row["template_id"].As<std::optional<boost::uuids::uuid>>()),
                             row["active"].As<bool>()
    );
  }
  return std::make_unique<schemas::RecipientGroupWithIdList>(groups_data);
}

std::unique_ptr<schemas::RecipientGroupWithIdList> ens::groups::GroupManager::GetAll(const boost::uuids::uuid &user_id) const {
  const userver::storages::postgres::Query group_info_query{
      "SELECT recipient_group_id, master_id, template_id, name, active "
      "FROM ens_schema.recipient_group "
      "WHERE master_id = $1",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        group_info_query,
                                        user_id);
  schemas::RecipientGroupWithIdList groups_data;
  for (auto row : select_res) {
    groups_data.emplace_back(boost::uuids::to_string(row["recipient_group_id"].As<boost::uuids::uuid>()),
                             boost::uuids::to_string(user_id),
                             row["name"].As<std::string>(),
                             ens::utils::optional_uuid_to_str(row["template_id"].As<std::optional<boost::uuids::uuid>>()),
                             row["active"].As<bool>()
    );
  }
  return std::make_unique<schemas::RecipientGroupWithIdList>(groups_data);
}

std::unique_ptr<schemas::RecipientGroupWithId> ens::groups::GroupManager::ConfirmCreation(const boost::uuids::uuid &user_id,
                                                                                          const boost::uuids::uuid &draft_id) {
  boost::uuids::uuid group_id = userver::utils::generators::GenerateBoostUuidV7();
  const userver::storages::postgres::Query group_creation_query{
      "INSERT INTO ens_schema.recipient_group "
      "(recipient_group_id, master_id, template_id, name, active) ( "
      "SELECT $3, $1, template_id, name, active "
      "FROM ens_schema.recipient_group_draft "
      "WHERE master_id = $1 AND recipient_group_draft_id = $2) "
      "RETURNING template_id, name, active"
  };
  const userver::storages::postgres::Query draft_deletion_query{
      "DELETE "
      "FROM ens_schema.notification_template_draft "
      "WHERE master_id = $1 AND notification_template_draft_id = $2",
  };
  userver::storages::postgres::Transaction confirmation_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      insertion_res = confirmation_transaction.Execute(group_creation_query,
                                                       user_id,
                                                       draft_id,
                                                       group_id);
  if (not insertion_res.RowsAffected()) {
    confirmation_transaction.Rollback();
    throw DraftNotFoundException{boost::uuids::to_string(draft_id)};
  }
  userver::storages::postgres::ResultSet
      deletion_res = confirmation_transaction.Execute(draft_deletion_query,
                                                      user_id,
                                                      draft_id);
  confirmation_transaction.Commit();
  userver::storages::postgres::Row group_row = insertion_res[0];
  schemas::RecipientGroupWithId group_data{boost::uuids::to_string(group_id),
                                           boost::uuids::to_string(user_id),
                                           group_row["name"].As<std::string>(),
                                           utils::optional_uuid_to_str(group_row["template_id"].As<std::optional<boost::uuids::uuid>>()),
                                           group_row["active"].As<bool>()
  };
  return std::make_unique<schemas::RecipientGroupWithId>(group_data);
}

std::unique_ptr<schemas::RecipientGroupWithId> ens::groups::GroupManager::ModifyGroup(const boost::uuids::uuid &user_id,
                                                                                      const boost::uuids::uuid &group_id,
                                                                                      const schemas::RecipientGroupWithoutId &data) {
  const userver::storages::postgres::Query update_query{
      "UPDATE ens_schema.recipient_group "
      "SET name = $3, template_id = $4, active = $5 "
      "WHERE master_id = $1 AND recipient_group_id = $2 "
      "RETURNING name, template_id, active"
  };
  userver::storages::postgres::Transaction update_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  try {
    userver::storages::postgres::ResultSet
        update_res = update_transaction.Execute(update_query,
                                                user_id,
                                                group_id,
                                                data.name,
                                                ens::utils::optional_str_to_uuid(data.notification_template_id),
                                                data.active);
    if (not update_res.RowsAffected()) {
      update_transaction.Rollback();
      throw RecipientGroupNotFoundException{boost::uuids::to_string(group_id)};
    }
    update_transaction.Commit();
    userver::storages::postgres::Row group_row = update_res[0];
    schemas::RecipientGroupWithId group_data{boost::uuids::to_string(group_id),
                                             boost::uuids::to_string(user_id),
                                             group_row["name"].As<std::string>(),
                                             ens::utils::optional_uuid_to_str(group_row["template_id"].As<std::optional<
                                                 boost::uuids::uuid>>()),
                                             group_row["active"].As<bool>()
    };
    return std::make_unique<schemas::RecipientGroupWithId>(group_data);
  }
  catch (const boost::bad_lexical_cast &e) { // means that template_id is not null but non-uuid
    throw IncorrectNotificationTemplateIdException{data.notification_template_id.value()};
  }
  catch (const userver::storages::postgres::ForeignKeyViolation &e) { // means that template_id is not null but does not exist
    throw IncorrectNotificationTemplateIdException{data.notification_template_id.value()};
  }
}

void ens::groups::GroupManager::AddRecipient(const boost::uuids::uuid &user_id,
                                             const boost::uuids::uuid &group_id,
                                             const boost::uuids::uuid &recipient_id) {
  const userver::storages::postgres::Query group_exists_query{
      "SELECT EXISTS ( "
      "SELECT 1 "
      "FROM ens_schema.recipient_group "
      "WHERE master_id = $1 AND recipient_group_id = $2)"
  };
  const userver::storages::postgres::Query recipient_exists_query{
      "SELECT EXISTS ( "
      "SELECT 1 "
      "FROM ens_schema.recipient "
      "WHERE master_id = $1 AND recipient_id = $2)"
  };
  const userver::storages::postgres::Query add_recipient_query{
      "INSERT INTO ens_schema.recipient_recipient_group "
      "(recipient_group_id, recipient_id) "
      "VALUES ($1, $2)"
  };
  userver::storages::postgres::Transaction transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet group_exists_res = transaction.Execute(group_exists_query,
                                                                                user_id,
                                                                                group_id);
  userver::storages::postgres::ResultSet recipient_exists_res = transaction.Execute(recipient_exists_query,
                                                                                    user_id,
                                                                                    recipient_id);
  if (not group_exists_res.AsSingleRow<bool>()) {
    transaction.Rollback();
    throw RecipientGroupNotFoundException{boost::uuids::to_string(group_id)};
  }
  if (not recipient_exists_res.AsSingleRow<bool>()) {
    transaction.Rollback();
    throw RecipientNotFoundException{boost::uuids::to_string(recipient_id)};
  }
  try {
    userver::storages::postgres::ResultSet insert_res = transaction.Execute(add_recipient_query,
                                                                            group_id,
                                                                            recipient_id);
    transaction.Commit();
  }
  catch (const userver::storages::postgres::UniqueViolation &e) {
    transaction.Rollback();
    throw RecipientAlreadyAddedException{boost::uuids::to_string(recipient_id),
                                         boost::uuids::to_string(group_id)};
  }
}

void ens::groups::GroupManager::DeleteGroup(const boost::uuids::uuid &user_id, const boost::uuids::uuid &group_id) {
  const userver::storages::postgres::Query delete_query{
      "DELETE "
      "FROM ens_schema.recipient_group "
      "WHERE master_id = $1 AND recipient_group_id = $2"
  };
  userver::storages::postgres::Transaction delete_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      delete_res = delete_transaction.Execute(delete_query,
                                              user_id,
                                              group_id);
  if (not delete_res.RowsAffected()) {
    delete_transaction.Rollback();
    throw RecipientGroupNotFoundException{boost::uuids::to_string(group_id)};
  }
  delete_transaction.Commit();
}

void ens::groups::GroupManager::DeleteRecipient(const boost::uuids::uuid &user_id,
                                                const boost::uuids::uuid &group_id,
                                                const boost::uuids::uuid &recipient_id) {
  const userver::storages::postgres::Query group_exists_query{
      "SELECT EXISTS ( "
      "SELECT 1 "
      "FROM ens_schema.recipient_group "
      "WHERE master_id = $1 AND recipient_group_id = $2)"
  };
  const userver::storages::postgres::Query recipient_exists_query{
      "SELECT EXISTS ( "
      "SELECT 1 "
      "FROM ens_schema.recipient "
      "WHERE master_id = $1 AND recipient_id = $2)"
  };
  const userver::storages::postgres::Query delete_query{
      "DELETE "
      "FROM ens_schema.recipient_recipient_group "
      "WHERE recipient_group_id = $1 AND recipient_id = $2"
  };
  userver::storages::postgres::Transaction transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet group_exists_res = transaction.Execute(group_exists_query,
                                                                                user_id,
                                                                                group_id);
  userver::storages::postgres::ResultSet recipient_exists_res = transaction.Execute(recipient_exists_query,
                                                                                    user_id,
                                                                                    recipient_id);
  if (not group_exists_res.AsSingleRow<bool>()) {
    transaction.Rollback();
    throw RecipientGroupNotFoundException{boost::uuids::to_string(group_id)};
  }
  if (not recipient_exists_res.AsSingleRow<bool>()) {
    transaction.Rollback();
    throw RecipientNotFoundException{boost::uuids::to_string(recipient_id)};
  }
  userver::storages::postgres::ResultSet
      delete_res = transaction.Execute(delete_query,
                                       group_id,
                                       recipient_id);
  if (not delete_res.RowsAffected()) {
    transaction.Rollback();
    throw RecipientNotAddedException{boost::uuids::to_string(recipient_id),
                                     boost::uuids::to_string(group_id)};
  }
  transaction.Commit();
}

void ens::groups::AppendGroupManager(userver::components::ComponentList &component_list) {
  component_list.Append<GroupManager>();
}
