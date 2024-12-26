import utils
from schemas import GroupDraftSchema, GroupWithIdSchema, GroupWithIdListSchema, RecipientWithIdListSchema


async def test_create_group_200(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_draft_id = \
        (await utils.create_template(service_client, "test_template_1", access_token=access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, template_draft_id, access_token)).json()[
        "notification_template_id"]
    response = await utils.create_group(service_client, name, active, template_id, access_token)
    resp_json = response.json()
    draft_id = resp_json["draft_id"]
    db_draft = await utils.db_get_group_draft(draft_id, pgsql)
    assert response.status == 200
    GroupDraftSchema(resp_json)
    assert db_draft is not None


async def test_get_group_200(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, name, active, access_token=access_token)).json()["draft_id"]
    group_id = (await utils.groups_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_group_id"]
    response = await utils.get_group(service_client, group_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    GroupWithIdSchema(resp_json)


async def test_get_active_groups_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    draft_id_2 = (await utils.create_group(service_client, "test_group_2", False, access_token=access_token)).json()[
        "draft_id"]
    await utils.groups_confirm_creation(service_client, draft_id_1, access_token)
    await utils.groups_confirm_creation(service_client, draft_id_2, access_token)
    response = await utils.get_active_groups(service_client, access_token)
    resp_json = response.json()
    assert response.status == 200
    GroupWithIdListSchema(resp_json)
    assert len(resp_json) == 1
    assert resp_json[0]["name"] == "test_group_1"


async def test_get_group_recipients_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    group_id = (await utils.groups_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_group_id"]
    recipient_draft_id_1 = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                         access_token=access_token)).json()["draft_id"]
    recipient_draft_id_2 = (await utils.create_recipient(service_client, "recipient_2", "example@example.com",
                                                         access_token=access_token)).json()["draft_id"]
    recipient_id_1 = (await utils.recipients_confirm_creation(service_client, recipient_draft_id_1,
                                                              access_token)).json()["recipient_id"]
    recipient_id_2 = (await utils.recipients_confirm_creation(service_client, recipient_draft_id_2,
                                                              access_token)).json()["recipient_id"]
    await utils.add_recipient_to_group(service_client, group_id, recipient_id_1, access_token)
    await utils.add_recipient_to_group(service_client, group_id, recipient_id_2, access_token)
    response = await utils.get_group_recipients(service_client, group_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    assert RecipientWithIdListSchema(resp_json)
    assert len(resp_json) == 2


async def test_get_groups_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    draft_id_2 = (await utils.create_group(service_client, "test_group_2", True, access_token=access_token)).json()[
        "draft_id"]
    await utils.groups_confirm_creation(service_client, draft_id_1, access_token)
    await utils.groups_confirm_creation(service_client, draft_id_2, access_token)
    response = await utils.get_groups(service_client, access_token)
    resp_json = response.json()
    assert response.status == 200
    GroupWithIdListSchema(resp_json)
    assert len(resp_json) == 2


async def test_confirm_group_creation_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    response = await utils.groups_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    group_id = resp_json["recipient_group_id"]
    db_group = await utils.db_get_group(group_id, pgsql)
    assert response.status == 200
    GroupWithIdSchema(resp_json)
    assert db_group is not None


async def test_modify_group_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    group_id = (await utils.groups_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_group_id"]
    old_db_data = await utils.db_get_group(group_id, pgsql)
    response = await utils.modify_group(service_client, group_id, "test_group_1", False, access_token=access_token)
    resp_json = response.json()
    new_db_data = await utils.db_get_group(group_id, pgsql)
    assert response.status == 200
    GroupWithIdSchema(resp_json)
    assert old_db_data != new_db_data
    assert not resp_json["active"]


async def test_group_add_recipient_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    group_id = (await utils.groups_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_group_id"]
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    db_group_recipients = await utils.db_get_group_recipients(group_id, 1, pgsql)
    assert response.status == 200
    assert len(db_group_recipients) == 1


async def test_group_delete_recipient_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    group_id = (await utils.groups_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_group_id"]
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    db_group_recipients = await utils.db_get_group_recipients(group_id, 1, pgsql)
    assert response.status == 200
    assert len(db_group_recipients) == 0


async def test_delete_group_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    group_id = (await utils.groups_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_group_id"]
    response = await utils.delete_group(service_client, group_id, access_token)
    db_groups = await utils.db_get_groups(1, pgsql)
    assert response.status == 200
    assert len(db_groups) == 0
