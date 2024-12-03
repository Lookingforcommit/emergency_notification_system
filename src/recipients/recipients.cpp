#include "recipients.hpp"

#include <memory>

#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/utils/boost_uuid7.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

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
      "(recipient_draft_id, master_id, name, email, phone_number, telegram_username) "
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
                                                                                 data.telegram_username);
  insert_transaction.Commit();
  schemas::RecipientDraft created_draft{boost::uuids::to_string(recipient_draft_id),
                                        boost::uuids::to_string(user_id),
                                        data.name,
                                        data.email,
                                        data.phone_number,
                                        data.telegram_username};
  return std::make_unique<schemas::RecipientDraft>(created_draft);
}

std::unique_ptr<schemas::RecipientWithId> ens::recipients::RecipientManager::GetById(const boost::uuids::uuid &user_id,
                                                                                     const std::string &recipient_id) const {
  const userver::storages::postgres::Query recipient_info_query{
      "SELECT recipient_id, master_id, name, email, phone_number, telegram_username "
      "FROM ens_schema.recipient "
      "WHERE master_id = $1 AND recipient_id = $2",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        recipient_info_query,
                                        user_id,
                                        boost::lexical_cast<boost::uuids::uuid>(recipient_id));
  if (select_res.IsEmpty()) {
    throw RecipientNotFoundException{recipient_id};
  }
  userver::storages::postgres::Row recipient_row = select_res[0];
  schemas::RecipientWithId recipient_data{recipient_id,
                                          boost::uuids::to_string(user_id),
                                          recipient_row["name"].As<std::string>(),
                                          recipient_row["email"].As<std::string>(),
                                          recipient_row["phone_number"].As<std::string>(),
                                          recipient_row["telegram_username"].As<std::string>()
  };
  return std::make_unique<schemas::RecipientWithId>(recipient_data);
}

std::unique_ptr<schemas::RecipientWithIdList> ens::recipients::RecipientManager::GetAll(const boost::uuids::uuid &user_id) const {
  const userver::storages::postgres::Query recipients_info_query{
      "SELECT recipient_id, master_id, name, email, phone_number, telegram_username "
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
        row["email"].As<std::string>(),
        row["phone_number"].As<std::string>(),
        row["telegram_username"].As<std::string>()
    );
  }
  return std::make_unique<schemas::RecipientWithIdList>(recipients_data);
}

std::unique_ptr<schemas::RecipientWithId> ens::recipients::RecipientManager::ConfirmCreation(const boost::uuids::uuid &user_id,
                                                                                             const std::string &draft_id) {
  boost::uuids::uuid recipient_id = userver::utils::generators::GenerateBoostUuidV7();
  const userver::storages::postgres::Query recipient_creation_query{
      "INSERT INTO ens_schema.recipient "
      "(recipient_id, master_id, name, email, phone_number, telegram_username) ( "
      "SELECT $3, $1, name, email, phone_number, telegram_username "
      "FROM ens_schema.recipient_draft "
      "WHERE master_id = $1 AND recipient_draft_id = $2) "
      "RETURNING recipient_id, master_id, name, email, phone_number, telegram_username"
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
                                                       boost::lexical_cast<boost::uuids::uuid>(user_id),
                                                       boost::lexical_cast<boost::uuids::uuid>(draft_id),
                                                       recipient_id);
  if (not insertion_res.RowsAffected()) {
    throw DraftNotFoundException{draft_id};
  }
  userver::storages::postgres::ResultSet
      deletion_res = confirmation_transaction.Execute(draft_deletion_query,
                                                      user_id,
                                                      boost::lexical_cast<boost::uuids::uuid>(draft_id));
  confirmation_transaction.Commit();
  userver::storages::postgres::Row recipient_row = insertion_res[0];
  schemas::RecipientWithId recipient_data{boost::uuids::to_string(recipient_id),
                                          boost::uuids::to_string(user_id),
                                          recipient_row["name"].As<std::string>(),
                                          recipient_row["email"].As<std::string>(),
                                          recipient_row["phone_number"].As<std::string>(),
                                          recipient_row["telegram_username"].As<std::string>()
  };
  return std::make_unique<schemas::RecipientWithId>(recipient_data);
}

std::unique_ptr<schemas::RecipientWithId> ens::recipients::RecipientManager::ModifyRecipient(const boost::uuids::uuid &user_id,
                                                                                             const std::string &recipient_id,
                                                                                             const schemas::RecipientWithoutId &data) {
  const userver::storages::postgres::Query update_query{
      "UPDATE ens_schema.recipient "
      "SET name = $3, email = $4, phone_number = $5, telegram_username = $6 "
      "WHERE master_id = $1 AND recipient_id = $2 "
      "RETURNING recipient_id, master_id, name, email, phone_number, telegram_username"
  };
  userver::storages::postgres::Transaction update_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      update_res = update_transaction.Execute(update_query,
                                              user_id,
                                              boost::lexical_cast<boost::uuids::uuid>(recipient_id),
                                              data.name,
                                              data.email,
                                              data.phone_number,
                                              data.telegram_username);
  if (not update_res.RowsAffected()) {
    throw RecipientNotFoundException{recipient_id};
  }
  update_transaction.Commit();
  userver::storages::postgres::Row recipient_row = update_res[0];
  schemas::RecipientWithId recipient_data{recipient_id,
                                          boost::uuids::to_string(user_id),
                                          recipient_row["name"].As<std::string>(),
                                          recipient_row["email"].As<std::string>(),
                                          recipient_row["phone_number"].As<std::string>(),
                                          recipient_row["telegram_username"].As<std::string>()
  };
  return std::make_unique<schemas::RecipientWithId>(recipient_data);
}

void ens::recipients::RecipientManager::DeleteRecipient(const boost::uuids::uuid &user_id, const std::string &recipient_id) {
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
                                              boost::lexical_cast<boost::uuids::uuid>(recipient_id));
  if (not delete_res.RowsAffected()) {
    throw RecipientNotFoundException{recipient_id};
  }
  delete_transaction.Commit();
}

void ens::recipients::AppendRecipientManager(userver::components::ComponentList &component_list) {
  component_list.Append<RecipientManager>();
}
