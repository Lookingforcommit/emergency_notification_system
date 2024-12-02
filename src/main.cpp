#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "users/handlers.hpp"
#include "recipients/handlers.hpp"
#include "utils.hpp"

int main(int argc, char *argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
      .Append<userver::server::handlers::Ping>()
      .Append<userver::components::TestsuiteSupport>()
      .Append<userver::clients::dns::Component>()
      .Append<userver::components::HttpClient>()
      .Append<userver::server::handlers::TestsControl>()
      .Append<userver::components::Postgres>(utils::DB_COMPONENT_NAME)
      .Append<userver::components::Secdist>()
      .Append<userver::components::DefaultSecdistProvider>();
  ens::users::AppendUserManager(component_list);
  ens::users::AppendJwtManager(component_list);
  ens::users::AppendUserCreateHandler(component_list);
  ens::users::AppendUserLoginHandler(component_list);
  ens::users::AppendUserModifyHandler(component_list);
  ens::users::AppendUserRefreshTokenHandler(component_list);
  ens::users::AppendUserDeleteHandler(component_list);
  ens::recipients::AppendRecipientsManager(component_list);
  ens::recipients::AppendRecipientCreateHandler(component_list);
  ens::recipients::AppendRecipientGetByIdHandler(component_list);
  ens::recipients::AppendRecipientGetAllHandler(component_list);
  ens::recipients::AppendRecipientConfirmCreationHandler(component_list);
  ens::recipients::AppendRecipientModifyHandler(component_list);
  ens::recipients::AppendRecipientDeleteHandler(component_list);
  return userver::utils::DaemonMain(argc, argv, component_list);
}
