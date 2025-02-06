#pragma once

#include <userver/telegram/bot/client/client_fwd.hpp>

#include "userver/components/component.hpp"
#include "userver/components/loggable_component_base.hpp"
#include "userver/utils/strong_typedef.hpp"
#include "userver/formats/json.hpp"

USERVER_NAMESPACE_BEGIN

namespace telegram::bot {

class BotSecdistConfig {
 public:
  using BotToken = userver::utils::NonLoggable<class BotTokenTag, std::string>;
  const std::string bot_token_;
  BotSecdistConfig(const userver::formats::json::Value &val)
      : bot_token_(val["bot_token"].As<BotToken>()) {}
};

class TelegramBotClient : public components::LoggableComponentBase {
public:
  static constexpr std::string_view kName = "telegram-bot-client";

  TelegramBotClient(const components::ComponentConfig& config,
                    const components::ComponentContext& context);

  static yaml_config::Schema GetStaticConfigSchema();

  ClientPtr GetClient();

private:
  BotSecdistConfig secdist_config_;
  ClientPtr client_;
};

}  // namespace telegram::bot

USERVER_NAMESPACE_END
