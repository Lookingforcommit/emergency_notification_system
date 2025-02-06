#include <userver/telegram/bot/components/client.hpp>

#include <telegram/bot/client/client_impl.hpp>

#include "userver/clients/http/component.hpp"
#include <userver/storages/secdist/component.hpp>
#include "userver/yaml_config/merge_schemas.hpp"

USERVER_NAMESPACE_BEGIN

namespace telegram::bot {

namespace {

constexpr std::string_view kDefaultApiBaseUrl = "https://api.telegram.org";

ClientPtr MakeClient(clients::http::Client& httpClient,
                     const components::ComponentConfig& config,
                     const BotSecdistConfig& secdist_config) {
  auto apiBaseURL = config["api-base-url"].As<std::string>(kDefaultApiBaseUrl);
  auto fileBaseUrl = config["file-base-url"].As<std::string>(apiBaseURL);

  return std::make_shared<ClientImpl>(httpClient,
                                      secdist_config.bot_token_,
                                      std::move(apiBaseURL),
                                      std::move(fileBaseUrl));
}

}  // namespace

TelegramBotClient::TelegramBotClient(
    const components::ComponentConfig& config,
    const components::ComponentContext& context)
    : LoggableComponentBase(config, context),
      secdist_config_(
          context.FindComponent<userver::components::Secdist>().Get().Get<BotSecdistConfig>()
      ),
      client_(MakeClient(
        context.FindComponent<components::HttpClient>().GetHttpClient(),
        config,
        secdist_config_)){}

ClientPtr TelegramBotClient::GetClient() {
    return client_;
}

yaml_config::Schema TelegramBotClient::GetStaticConfigSchema() {
//  bot-token:
//  type: string
//  description: Telegram bot authentication token.
  return yaml_config::MergeSchemas<components::LoggableComponentBase>(R"(
type: object
description: Telegram Bot Client component
additionalProperties: false
properties:
    api-base-url:
        type: string
        description: URL to the server implementing the API. It makes sense to change in case of using a local server or for testing.
        defaultDescription: "https://api.telegram.org"
    file-base-url:
        type: string
        description: URL to the service providing file downloads.
        defaultDescription: Same as api-base-url
)");
}

}  // namespace telegram::bot

USERVER_NAMESPACE_END
