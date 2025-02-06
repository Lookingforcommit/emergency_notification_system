#include "telegram_bot.hpp"

#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/telegram/bot/requests/send_message.hpp>

userver::yaml_config::Schema ens::notifications::telegram::TelegramNotificationsBot::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::telegram::bot::TelegramBotLongPoller>(R"(
    type: object
    description: Component for telegram notifier bot
    additionalProperties: false
    properties: {}
  )");
}

void ens::notifications::telegram::TelegramNotificationsBot::SendMessage(const userver::telegram::bot::ChatId &chat_id,
                                                                         const std::string &msg_text) {
  using namespace userver::telegram::bot;
  const SendMessageMethod::Parameters msg_params{chat_id, msg_text};
  Request<SendMessageMethod> sent_msg = this->GetClient()->SendMessage(msg_params,
                                                                       userver::telegram::bot::RequestOptions{});
  sent_msg.Perform();
}

void ens::notifications::telegram::TelegramNotificationsBot::HandleHelp(userver::telegram::bot::Update &update) {
  SendMessage(update.message->chat->id, HELP_MESSAGE);
}

void ens::notifications::telegram::TelegramNotificationsBot::HandleSendNotifications(userver::telegram::bot::Update &update,
                                                                                     const int64_t user_id) {
  using namespace userver::telegram::bot;
  const userver::storages::postgres::Query contact_exists_query{
      "SELECT EXISTS ( "
      "SELECT 1 "
      "FROM ens_schema.telegram_contact "
      "WHERE user_id = $1)"
  };
  const userver::storages::postgres::Query create_query{
      "INSERT INTO ens_schema.telegram_contact "
      "(user_id, active) "
      "VALUES ($1, true)"
  };
  const userver::storages::postgres::Query update_query{
      "UPDATE ens_schema.telegram_contact "
      "SET active = true "
      "WHERE user_id = $1 AND active = false"
  };
  userver::storages::postgres::ResultSet
      contact_exists_res = _pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                                contact_exists_query,
                                                user_id);
  userver::storages::postgres::Transaction upsert_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  std::string msg;
  if (not contact_exists_res.AsSingleRow<bool>()) {
    upsert_transaction.Execute(create_query,
                               user_id);
    msg = "Success! Now you will receive notifications from other users";
    upsert_transaction.Commit();
  } else {
    const userver::storages::postgres::ResultSet update_res = upsert_transaction.Execute(update_query,
                                                                                         user_id);
    if (not update_res.RowsAffected()) {
      msg = "You are already subscribed to notifications receiving";
      upsert_transaction.Rollback();
    } else {
      msg = "Success! Now you will receive notifications from other users";
      upsert_transaction.Commit();
    }
  }
  SendMessage(update.message->chat->id, msg);
}

void ens::notifications::telegram::TelegramNotificationsBot::HandleStopNotifications(userver::telegram::bot::Update &update,
                                                                                     const int64_t user_id) {
  using namespace userver::telegram::bot;
  const userver::storages::postgres::Query update_query{
      "UPDATE ens_schema.telegram_contact "
      "SET active = false "
      "WHERE user_id = $1 AND active = true"
  };
  userver::storages::postgres::Transaction update_transaction =
      _pg_cluster->Begin(userver::storages::postgres::ClusterHostType::kMaster, {});
  userver::storages::postgres::ResultSet update_res = update_transaction.Execute(update_query,
                                                                                 user_id);
  std::string msg;
  if (not update_res.RowsAffected()) {
    update_transaction.Rollback();
    msg = "You aren't subscribed to notifications receiving";
  } else {
    update_transaction.Commit();
    msg = "Stopped notifications receiving";
  }
  SendMessage(update.message->chat->id, msg);
}

void ens::notifications::telegram::TelegramNotificationsBot::HandleUpdate(userver::telegram::bot::Update update,
                                                                          userver::telegram::bot::ClientPtr) {
  using namespace userver::telegram::bot;
  if (not update.message->text.has_value()) {
    return;
  }
  const std::string message_text = update.message->text.value();
  if (BOT_COMMANDS_STRING.find(message_text) == BOT_COMMANDS_STRING.cend()) {
    HandleHelp(update);
    return;
  }
  const BotCommands command = BOT_COMMANDS_STRING.find(message_text)->second;
  const int64_t user_id = update.message->chat->id;
  switch (command) {
    case BotCommands::Start:
    case BotCommands::Help: {
      HandleHelp(update);
      break;
    }
    case BotCommands::SendNotifications: {
      HandleSendNotifications(update, user_id);
      break;
    }
    case BotCommands::StopNotifications: {
      HandleStopNotifications(update, user_id);
      break;
    }
  }
}

void ens::notifications::telegram::AppendTelegramNotificationsBot(userver::components::ComponentList &component_list) {
  component_list.Append<TelegramNotificationsBot>();
}
