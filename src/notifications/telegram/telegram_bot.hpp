#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/telegram/bot/components/long_poller.hpp>
#include <userver/telegram/bot/types/update.hpp>
#include <userver/telegram/bot/client/client.hpp>
#include <utils/utils.hpp>

// TODO: Add setCommands method

namespace ens::notifications::telegram {
enum class BotCommands {
  Start,
  Help,
  SendNotifications,
  StopNotifications
};

const std::unordered_map<std::string, BotCommands> BOT_COMMANDS_STRING{
    {"/start", BotCommands::Start},
    {"/help", BotCommands::Help},
    {"/send_notifications", BotCommands::SendNotifications},
    {"/stop_notifications", BotCommands::StopNotifications}
};

const std::string HELP_MESSAGE{"This is a notifier bot for emergency_notification_system "
                               "(https://github.com/Lookingforcommit/emergency_notification_system) "
                               "use commands /send_notifications or /stop_notifications to accept/reject notifications "
                               "from other users"};

class TelegramNotificationsBot : public userver::telegram::bot::TelegramBotLongPoller {
 public:
  static constexpr std::string_view kName = "telegram-notifications-bot";
  TelegramNotificationsBot(const userver::components::ComponentConfig &config,
                           const userver::components::ComponentContext &component_context) :
      userver::telegram::bot::TelegramBotLongPoller(config, component_context),
      _pg_cluster(
          component_context
              .FindComponent<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
              .GetCluster()) {}
  static userver::yaml_config::Schema GetStaticConfigSchema();
  void SendMessage(const userver::telegram::bot::ChatId &chat_id,
                   const std::string &msg_text);
  void HandleHelp(userver::telegram::bot::Update &update);
  void HandleSendNotifications(userver::telegram::bot::Update &update,
                               const int64_t user_id);
  void HandleStopNotifications(userver::telegram::bot::Update &update,
                               const int64_t user_id);
  void HandleUpdate(userver::telegram::bot::Update update,
                    userver::telegram::bot::ClientPtr);
 private:
  userver::storages::postgres::ClusterPtr _pg_cluster;
};

void AppendTelegramNotificationsBot(userver::components::ComponentList &component_list);
}

