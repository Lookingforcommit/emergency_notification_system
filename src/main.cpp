#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "user/handlers.hpp"
#include "user/auth.hpp"
#include "user/user.hpp"
#include "recipients/handlers.hpp"
#include "recipients/recipients.hpp"
#include "templates/handlers.hpp"
#include "templates/templates.hpp"
#include "groups/groups.hpp"
#include "groups/handlers.hpp"
#include "utils/utils.hpp"

int main(int argc, char *argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
      .Append<userver::server::handlers::Ping>()
      .Append<userver::components::TestsuiteSupport>()
      .Append<userver::clients::dns::Component>()
      .Append<userver::components::HttpClient>()
      .Append<userver::server::handlers::TestsControl>()
      .Append<userver::components::Postgres>(ens::utils::DB_COMPONENT_NAME)
      .Append<userver::components::Secdist>()
      .Append<userver::components::DefaultSecdistProvider>();
  ens::user::AppendUserManager(component_list);
  ens::user::AppendJwtManager(component_list);
  ens::user::AppendUserCreateHandler(component_list);
  ens::user::AppendUserLoginHandler(component_list);
  ens::user::AppendUserModifyHandler(component_list);
  ens::user::AppendUserRefreshTokenHandler(component_list);
  ens::user::AppendUserDeleteHandler(component_list);
  ens::recipients::AppendRecipientManager(component_list);
  ens::recipients::AppendRecipientCreateHandler(component_list);
  ens::recipients::AppendRecipientGetByIdHandler(component_list);
  ens::recipients::AppendRecipientGetAllHandler(component_list);
  ens::recipients::AppendRecipientConfirmCreationHandler(component_list);
  ens::recipients::AppendRecipientModifyHandler(component_list);
  ens::recipients::AppendRecipientDeleteHandler(component_list);
  ens::templates::AppendTemplateManager(component_list);
  ens::templates::AppendTemplateCreateHandler(component_list);
  ens::templates::AppendTemplateGetByIdHandler(component_list);
  ens::templates::AppendTemplateGetAllHandler(component_list);
  ens::templates::AppendTemplateConfirmCreationHandler(component_list);
  ens::templates::AppendTemplateModifyHandler(component_list);
  ens::templates::AppendTemplateDeleteHandler(component_list);
  ens::groups::AppendGroupManager(component_list);
  ens::groups::AppendGroupCreateHandler(component_list);
  ens::groups::AppendGroupGetByIdHandler(component_list);
  ens::groups::AppendGroupGetRecipientsHandler(component_list);
  ens::groups::AppendGroupGetActiveHandler(component_list);
  ens::groups::AppendGroupGetAllHandler(component_list);
  ens::groups::AppendGroupConfirmCreationHandler(component_list);
  ens::groups::AppendGroupModifyGroupHandler(component_list);
  ens::groups::AppendGroupAddRecipientHandler(component_list);
  ens::groups::AppendGroupDeleteRecipientHandler(component_list);
  ens::groups::AppendGroupDeleteGroupHandler(component_list);
  return userver::utils::DaemonMain(argc, argv, component_list);
}
