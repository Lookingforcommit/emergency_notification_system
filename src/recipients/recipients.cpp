#include "recipients.hpp"

#include <memory>

#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/utils/boost_uuid7.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "schemas/schemas.hpp"

userver::yaml_config::Schema ens::recipients::RecipientManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: Component for recipients management logic
    additionalProperties: false
    properties: {}
  )");
}

std::unique_ptr<schemas::RecipientDraft> ens::recipients::RecipientManager::Create(const boost::uuids::uuid &user_id,
                                                                                   const schemas::RecipientWithoutId &data) {
  const userver::storages::postgres::Query create_recipient_query{
      "INSERT INTO ens_schema.recipient_draft "
      "(recipient_draft_id, master_id, name, email, phone_number, telegram_id) "
      "VALUES ($1, $2, $3, $4, $5, $6)"
  };
  boost::uuids::uuid recipient_draft_id = userver::utils::generators::GenerateBoostUuidV7();
  userver::storages::postgres::Transaction insert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet insert_res = insert_transaction.Execute(create_recipient_query,
                                                                                 recipient_draft_id,
                                                                                 user_id,
                                                                                 data.name,
                                                                                 data.email,
                                                                                 data.phone_number,
                                                                                 data.telegram_id);
  insert_transaction.Commit();
  schemas::RecipientDraft created_draft{boost::uuids::to_string(recipient_draft_id),
                                        boost::uuids::to_string(user_id),
                                        data.name,
                                        data.email,
                                        data.phone_number,
                                        data.telegram_id};
  return std::make_unique<schemas::RecipientDraft>(created_draft);
}

std::unique_ptr<schemas::RecipientWithId> ens::recipients::RecipientManager::GetById(const boost::uuids::uuid &user_id,
                                                                                     const boost::uuids::uuid &recipient_id) const {
  const userver::storages::postgres::Query recipient_info_query{
      "SELECT recipient_id, master_id, name, email, phone_number, telegram_id "
      "FROM ens_schema.recipient "
      "WHERE master_id = $1 AND recipient_id = $2",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        recipient_info_query,
                                        user_id,
                                        recipient_id);
  if (select_res.IsEmpty()) {
    throw RecipientNotFoundException{boost::uuids::to_string(recipient_id)};
  }
  userver::storages::postgres::Row recipient_row = select_res[0];
  schemas::RecipientWithId recipient_data{boost::uuids::to_string(recipient_id),
                                          boost::uuids::to_string(user_id),
                                          recipient_row["name"].As<std::string>(),
                                          recipient_row["email"].As<std::optional<std::string>>(),
                                          recipient_row["phone_number"].As<std::optional<std::string>>(),
                                          recipient_row["telegram_id"].As<std::optional<int64_t>>()
  };
  return std::make_unique<schemas::RecipientWithId>(recipient_data);
}

std::unique_ptr<schemas::RecipientWithIdList> ens::recipients::RecipientManager::GetAll(const boost::uuids::uuid &user_id) const {
  const userver::storages::postgres::Query recipients_info_query{
      "SELECT recipient_id, master_id, name, email, phone_number, telegram_id "
      "FROM ens_schema.recipient "
      "WHERE master_id = $1",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        recipients_info_query,
                                        user_id);
  schemas::RecipientWithIdList recipients_data;
  for (auto row : select_res) {
    recipients_data.emplace_back(
        boost::uuids::to_string(row["recipient_id"].As<boost::uuids::uuid>()),
        boost::uuids::to_string(user_id),
        row["name"].As<std::string>(),
        row["email"].As<std::optional<std::string>>(),
        row["phone_number"].As<std::optional<std::string>>(),
        row["telegram_id"].As<std::optional<int64_t>>()
    );
  }
  return std::make_unique<schemas::RecipientWithIdList>(recipients_data);
}

std::unique_ptr<schemas::RecipientWithId> ens::recipients::RecipientManager::ConfirmCreation(const boost::uuids::uuid &user_id,
                                                                                             const boost::uuids::uuid &draft_id) {
  boost::uuids::uuid recipient_id = userver::utils::generators::GenerateBoostUuidV7();
  const userver::storages::postgres::Query recipient_creation_query{
      "INSERT INTO ens_schema.recipient "
      "(recipient_id, master_id, name, email, phone_number, telegram_id) ( "
      "SELECT $3, $1, name, email, phone_number, telegram_id "
      "FROM ens_schema.recipient_draft "
      "WHERE master_id = $1 AND recipient_draft_id = $2) "
      "RETURNING name, email, phone_number, telegram_id"
  };
  const userver::storages::postgres::Query draft_deletion_query{
      "DELETE "
      "FROM ens_schema.recipient_draft "
      "WHERE master_id = $1 AND recipient_draft_id = $2",
  };
  userver::storages::postgres::Transaction confirmation_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      insertion_res = confirmation_transaction.Execute(recipient_creation_query,
                                                       user_id,
                                                       draft_id,
                                                       recipient_id);
  if (not insertion_res.RowsAffected()) {
    throw DraftNotFoundException{boost::uuids::to_string(draft_id)};
  }
  userver::storages::postgres::ResultSet
      deletion_res = confirmation_transaction.Execute(draft_deletion_query,
                                                      user_id,
                                                      draft_id);
  confirmation_transaction.Commit();
  userver::storages::postgres::Row recipient_row = insertion_res[0];
  schemas::RecipientWithId recipient_data{boost::uuids::to_string(recipient_id),
                                          boost::uuids::to_string(user_id),
                                          recipient_row["name"].As<std::string>(),
                                          recipient_row["email"].As<std::optional<std::string>>(),
                                          recipient_row["phone_number"].As<std::optional<std::string>>(),
                                          recipient_row["telegram_id"].As<std::optional<int64_t>>()
  };
  return std::make_unique<schemas::RecipientWithId>(recipient_data);
}

std::unique_ptr<schemas::RecipientWithId> ens::recipients::RecipientManager::ModifyRecipient(const boost::uuids::uuid &user_id,
                                                                                             const boost::uuids::uuid &recipient_id,
                                                                                             const schemas::RecipientWithoutId &data) {
  const userver::storages::postgres::Query update_query{
      "UPDATE ens_schema.recipient "
      "SET name = $3, email = $4, phone_number = $5, telegram_id = $6 "
      "WHERE master_id = $1 AND recipient_id = $2 "
      "RETURNING name, email, phone_number, telegram_id"
  };
  userver::storages::postgres::Transaction update_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      update_res = update_transaction.Execute(update_query,
                                              user_id,
                                              recipient_id,
                                              data.name,
                                              data.email,
                                              data.phone_number,
                                              data.telegram_id);
  if (not update_res.RowsAffected()) {
    throw RecipientNotFoundException{boost::uuids::to_string(recipient_id)};
  }
  update_transaction.Commit();
  userver::storages::postgres::Row recipient_row = update_res[0];
  schemas::RecipientWithId recipient_data{boost::uuids::to_string(recipient_id),
                                          boost::uuids::to_string(user_id),
                                          recipient_row["name"].As<std::string>(),
                                          recipient_row["email"].As<std::optional<std::string>>(),
                                          recipient_row["phone_number"].As<std::optional<std::string>>(),
                                          recipient_row["telegram_id"].As<std::optional<int64_t>>()
  };
  return std::make_unique<schemas::RecipientWithId>(recipient_data);
}

void ens::recipients::RecipientManager::DeleteRecipient(const boost::uuids::uuid &user_id,
                                                        const boost::uuids::uuid &recipient_id) {
  const userver::storages::postgres::Query delete_query{
      "DELETE "
      "FROM ens_schema.recipient "
      "WHERE master_id = $1 AND recipient_id = $2"
  };
  userver::storages::postgres::Transaction delete_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      delete_res = delete_transaction.Execute(delete_query,
                                              user_id,
                                              recipient_id);
  if (not delete_res.RowsAffected()) {
    throw RecipientNotFoundException{boost::uuids::to_string(recipient_id)};
  }
  delete_transaction.Commit();
}

void ens::recipients::AppendRecipientManager(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientManager>();
}
