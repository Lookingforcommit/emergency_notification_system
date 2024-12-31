#include "templates.hpp"

#include <memory>

#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/utils/boost_uuid7.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "schemas/schemas.hpp"

userver::yaml_config::Schema ens::templates::TemplateManager::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: Component for templates management logic
    additionalProperties: false
    properties: {}
  )");
}

std::unique_ptr<schemas::NotificationTemplateDraft> ens::templates::TemplateManager::Create(const boost::uuids::uuid &user_id,
                                                                                            const schemas::NotificationTemplateWithoutId &data) {
  const userver::storages::postgres::Query create_template_query{
      "INSERT INTO ens_schema.notification_template_draft "
      "(notification_template_draft_id, master_id, name, message_text) "
      "VALUES ($1, $2, $3, $4)"
  };
  boost::uuids::uuid template_draft_id = userver::utils::generators::GenerateBoostUuidV7();
  userver::storages::postgres::Transaction insert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet insert_res = insert_transaction.Execute(create_template_query,
                                                                                 template_draft_id,
                                                                                 user_id,
                                                                                 data.name,
                                                                                 data.message_text);
  insert_transaction.Commit();
  schemas::NotificationTemplateDraft created_draft{boost::uuids::to_string(template_draft_id),
                                                   boost::uuids::to_string(user_id),
                                                   data.name,
                                                   data.message_text};
  return std::make_unique<schemas::NotificationTemplateDraft>(created_draft);
}

std::unique_ptr<schemas::NotificationTemplateWithId> ens::templates::TemplateManager::GetById(const boost::uuids::uuid &user_id,
                                                                                              const boost::uuids::uuid &template_id) const {
  const userver::storages::postgres::Query template_info_query{
      "SELECT notification_template_id, master_id, name, message_text "
      "FROM ens_schema.notification_template "
      "WHERE master_id = $1 AND notification_template_id = $2",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        template_info_query,
                                        user_id,
                                        template_id);
  if (select_res.IsEmpty()) {
    throw NotificationTemplateNotFoundException{boost::uuids::to_string(template_id)};
  }
  userver::storages::postgres::Row template_row = select_res[0];
  schemas::NotificationTemplateWithId template_data{boost::uuids::to_string(template_id),
                                                    boost::uuids::to_string(user_id),
                                                    template_row["name"].As<std::string>(),
                                                    template_row["message_text"].As<std::optional<std::string>>()
  };
  return std::make_unique<schemas::NotificationTemplateWithId>(template_data);
}

std::unique_ptr<schemas::NotificationTemplateWithIdList> ens::templates::TemplateManager::GetAll(const boost::uuids::uuid &user_id) const {
  const userver::storages::postgres::Query templates_info_query{
      "SELECT notification_template_id, master_id, name, message_text "
      "FROM ens_schema.notification_template "
      "WHERE master_id = $1",
  };
  userver::storages::postgres::ResultSet
      select_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                        templates_info_query,
                                        user_id);
  schemas::NotificationTemplateWithIdList templates_data;
  for (auto row : select_res) {
    templates_data.emplace_back(
        boost::uuids::to_string(row["notification_template_id"].As<boost::uuids::uuid>()),
        boost::uuids::to_string(user_id),
        row["name"].As<std::string>(),
        row["message_text"].As<std::optional<std::string>>()
    );
  }
  return std::make_unique<schemas::NotificationTemplateWithIdList>(templates_data);
}

std::unique_ptr<schemas::NotificationTemplateWithId> ens::templates::TemplateManager::ConfirmCreation(const boost::uuids::uuid &user_id,
                                                                                                      const boost::uuids::uuid &draft_id) {
  boost::uuids::uuid template_id = userver::utils::generators::GenerateBoostUuidV7();
  const userver::storages::postgres::Query template_creation_query{
      "INSERT INTO ens_schema.notification_template "
      "(notification_template_id, master_id, name, message_text) ( "
      "SELECT $3, $1, name, message_text "
      "FROM ens_schema.notification_template_draft "
      "WHERE master_id = $1 AND notification_template_draft_id = $2) "
      "RETURNING name, message_text"
  };
  const userver::storages::postgres::Query draft_deletion_query{
      "DELETE "
      "FROM ens_schema.notification_template_draft "
      "WHERE master_id = $1 AND notification_template_draft_id = $2",
  };
  userver::storages::postgres::Transaction confirmation_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      insertion_res = confirmation_transaction.Execute(template_creation_query,
                                                       user_id,
                                                       draft_id,
                                                       template_id);
  if (not insertion_res.RowsAffected()) {
    throw DraftNotFoundException{boost::uuids::to_string(draft_id)};
  }
  userver::storages::postgres::ResultSet
      deletion_res = confirmation_transaction.Execute(draft_deletion_query,
                                                      user_id,
                                                      draft_id);
  confirmation_transaction.Commit();
  userver::storages::postgres::Row template_row = insertion_res[0];
  schemas::NotificationTemplateWithId template_data{boost::uuids::to_string(template_id),
                                                    boost::uuids::to_string(user_id),
                                                    template_row["name"].As<std::string>(),
                                                    template_row["message_text"].As<std::optional<std::string>>()
  };
  return std::make_unique<schemas::NotificationTemplateWithId>(template_data);
}

std::unique_ptr<schemas::NotificationTemplateWithId> ens::templates::TemplateManager::ModifyTemplate(const boost::uuids::uuid &user_id,
                                                                                                     const boost::uuids::uuid &template_id,
                                                                                                     const schemas::NotificationTemplateWithoutId &data) {
  const userver::storages::postgres::Query update_query{
      "UPDATE ens_schema.notification_template "
      "SET name = $3, message_text = $4 "
      "WHERE master_id = $1 AND notification_template_id = $2 "
      "RETURNING name, message_text"
  };
  userver::storages::postgres::Transaction update_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      update_res = update_transaction.Execute(update_query,
                                              user_id,
                                              template_id,
                                              data.name,
                                              data.message_text);
  if (not update_res.RowsAffected()) {
    throw NotificationTemplateNotFoundException{boost::uuids::to_string(template_id)};
  }
  update_transaction.Commit();
  userver::storages::postgres::Row template_row = update_res[0];
  schemas::NotificationTemplateWithId template_data{boost::uuids::to_string(template_id),
                                                    boost::uuids::to_string(user_id),
                                                    template_row["name"].As<std::string>(),
                                                    template_row["message_text"].As<std::optional<std::string>>()
  };
  return std::make_unique<schemas::NotificationTemplateWithId>(template_data);
}

void ens::templates::TemplateManager::DeleteTemplate(const boost::uuids::uuid &user_id,
                                                     const boost::uuids::uuid &template_id) {
  const userver::storages::postgres::Query delete_query{
      "DELETE "
      "FROM ens_schema.notification_template "
      "WHERE master_id = $1 AND notification_template_id = $2"
  };
  userver::storages::postgres::Transaction delete_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet
      delete_res = delete_transaction.Execute(delete_query,
                                              user_id,
                                              template_id);
  if (not delete_res.RowsAffected()) {
    throw NotificationTemplateNotFoundException{boost::uuids::to_string(template_id)};
  }
  delete_transaction.Commit();
}

void ens::templates::AppendTemplateManager(userver::components::ComponentList &component_list) {
  component_list.Append<TemplateManager>();
}
