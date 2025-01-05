import json
import typing

import utils
from schemas import GroupDraftSchema, GroupWithIdSchema, GroupWithIdListSchema, RecipientWithIdListSchema


async def create_group_w_confirmation(service_client, name: str = "", active: bool = "",
                                      template_id: str = "") -> typing.Tuple[str, str]:
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, name, active, template_id,
                                         access_token=access_token)).json()["draft_id"]
    group_id = (await utils.groups_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_group_id"]
    return access_token, group_id


async def test_create_group_200(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_draft_id = (await utils.create_template(service_client, "test_template_1",
                                                     access_token=access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, template_draft_id, access_token)).json()[
        "notification_template_id"]
    response = await utils.create_group(service_client, name, active, template_id, access_token)
    resp_json = response.json()
    draft_id = resp_json["draft_id"]
    db_draft = await utils.db_get_group_draft(draft_id, pgsql)
    assert response.status == 200
    GroupDraftSchema(resp_json)
    assert db_draft is not None


async def test_create_group_401_missing_token(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_draft_id = (await utils.create_template(service_client, "test_template_1",
                                                     access_token=access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, template_draft_id, access_token)).json()[
        "notification_template_id"]
    response = await utils.create_group(service_client, name, active, template_id)
    resp_json = response.json()
    db_drafts = await utils.db_get_group_drafts(1, pgsql)
    assert response.status == 401
    assert "draft_id" not in resp_json
    assert len(db_drafts) == 0


async def test_create_group_401_incorrect_user_id(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_draft_id = (await utils.create_template(service_client, "test_template_1",
                                                     access_token=access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, template_draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.create_group(service_client, name, active, template_id)
    resp_json = response.json()
    db_drafts = await utils.db_get_group_drafts(1, pgsql)
    assert response.status == 401
    assert "draft_id" not in resp_json
    assert len(db_drafts) == 0


async def test_create_group_422_extra_property(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    payload = {
        "name": name,
        "active": active,
        "extra_property": "smth"
    }
    response = await service_client.post(
        '/groups/create',
        data=json.dumps(payload),
        headers={"Authorization": access_token}
    )
    db_drafts = await utils.db_get_group_drafts(1, pgsql)
    resp_json = response.json()
    assert response.status == 422
    assert "draft_id" not in resp_json
    assert len(db_drafts) == 0


async def test_create_group_422_non_uuid_template_id(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_id = "incorrect notification_template_id"
    response = await utils.create_group(service_client, name, active, template_id, access_token)
    db_drafts = await utils.db_get_group_drafts(1, pgsql)
    resp_json = response.json()
    assert response.status == 422
    assert "draft_id" not in resp_json
    assert len(db_drafts) == 0


async def test_create_group_422_incorrect_template_id(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_draft_id = (await utils.create_template(service_client, "test_template_1",
                                                     access_token=access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, template_draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_template(service_client, template_id, access_token)
    response = await utils.create_group(service_client, name, active, template_id, access_token)
    db_drafts = await utils.db_get_group_drafts(1, pgsql)
    resp_json = response.json()
    assert response.status == 422
    assert "draft_id" not in resp_json
    assert len(db_drafts) == 0


async def test_get_group_200(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    response = await utils.get_group(service_client, group_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    GroupWithIdSchema(resp_json)


async def test_get_group_401_missing_access_token(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    response = await utils.get_group(service_client, group_id)
    assert response.status == 401
    assert "recipient_group_id" not in response.json()


async def test_get_group_401_incorrect_user_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_group(service_client, group_id, access_token)
    assert response.status == 401
    assert "recipient_group_id" not in response.json()


async def test_get_group_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = "incorrect group_id"
    response = await utils.get_group(service_client, group_id, access_token)
    assert response.status == 404
    assert "recipient_group_id" not in response.json()


async def test_get_group_404_missing_group_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = ""
    response = await utils.get_group(service_client, group_id, access_token)
    assert response.status == 404
    assert "recipient_group_id" not in response.json()


async def test_get_group_404_incorrect_group_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    await utils.delete_group(service_client, group_id, access_token)
    response = await utils.get_group(service_client, group_id, access_token)
    assert response.status == 404
    assert "recipient_group_id" not in response.json()


async def test_get_active_groups_200(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    for i in range(2):
        draft_id = (await utils.create_group(service_client, f"test_group_{i}", True,
                                             access_token=access_token)).json()["draft_id"]
        await utils.groups_confirm_creation(service_client, draft_id, access_token)
    response = await utils.get_active_groups(service_client, access_token)
    resp_json = response.json()
    assert response.status == 200
    GroupWithIdListSchema(resp_json)
    assert len(resp_json) == 2


async def test_get_active_groups_401_missing_token(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    for i in range(2):
        draft_id = (await utils.create_group(service_client, f"test_group_{i}", True,
                                             access_token=access_token)).json()["draft_id"]
        await utils.groups_confirm_creation(service_client, draft_id, access_token)
    response = await utils.get_active_groups(service_client)
    assert response.status == 401


async def test_get_active_groups_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    draft_id_2 = (await utils.create_group(service_client, "test_group_2", False, access_token=access_token)).json()[
        "draft_id"]
    await utils.groups_confirm_creation(service_client, draft_id_1, access_token)
    await utils.groups_confirm_creation(service_client, draft_id_2, access_token)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_active_groups(service_client, access_token)
    assert response.status == 401


async def test_get_group_recipients_200(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    for i in range(2):
        draft_id = (await utils.create_recipient(service_client, f"recipient_{i}", "example@example.com",
                                                 access_token=access_token)).json()["draft_id"]
        recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id,
                                                                access_token)).json()["recipient_id"]
        await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.get_group_recipients(service_client, group_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    assert RecipientWithIdListSchema(resp_json)
    assert len(resp_json) == 2


async def test_get_group_recipients_401_missing_token(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    for i in range(2):
        draft_id = (await utils.create_recipient(service_client, f"recipient_{i}", "example@example.com",
                                                 access_token=access_token)).json()["draft_id"]
        recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id,
                                                                access_token)).json()["recipient_id"]
        await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.get_group_recipients(service_client, group_id)
    assert response.status == 401


async def test_get_group_recipients_401_incorrect_user_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    for i in range(2):
        draft_id = (await utils.create_recipient(service_client, f"recipient_{i}", "example@example.com",
                                                 access_token=access_token)).json()["draft_id"]
        recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id,
                                                                access_token)).json()["recipient_id"]
        await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_group_recipients(service_client, group_id, access_token)
    assert response.status == 401


async def test_get_group_recipients_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = "incorrect group_id"
    response = await utils.get_group_recipients(service_client, group_id, access_token)
    assert response.status == 404


async def test_get_group_recipients_404_missing_group_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = ""
    response = await utils.get_group_recipients(service_client, group_id, access_token)
    assert response.status == 404


async def test_get_group_recipients_404_incorrect_group_id(service_client):
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
    await utils.delete_group(service_client, group_id, access_token)
    response = await utils.get_group_recipients(service_client, group_id, access_token)
    assert response.status == 404


async def test_get_groups_200(service_client):
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


async def test_get_groups_401_missing_token(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    draft_id_2 = (await utils.create_group(service_client, "test_group_2", True, access_token=access_token)).json()[
        "draft_id"]
    await utils.groups_confirm_creation(service_client, draft_id_1, access_token)
    await utils.groups_confirm_creation(service_client, draft_id_2, access_token)
    response = await utils.get_groups(service_client)
    assert response.status == 401


async def test_get_groups_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    for i in range(2):
        draft_id = (await utils.create_group(service_client, f"test_group_{i}", True,
                                             access_token=access_token)).json()["draft_id"]
        await utils.groups_confirm_creation(service_client, draft_id, access_token)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_groups(service_client, access_token)
    assert response.status == 401


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


async def test_confirm_group_creation_401_missing_token(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    response = await utils.groups_confirm_creation(service_client, draft_id)
    resp_json = response.json()
    assert response.status == 401
    assert "recipient_group_id" not in resp_json


async def test_confirm_group_creation_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.groups_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 401
    assert "recipient_group_id" not in resp_json


async def test_confirm_group_creation_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = "incorrect draft_id"
    response = await utils.groups_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_group_id" not in resp_json


async def test_confirm_group_creation_404_missing_draft_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = ""
    response = await utils.groups_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_group_id" not in resp_json


async def test_confirm_group_creation_404_incorrect_draft_id(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_group(service_client, "test_group_1", True, access_token=access_token)).json()[
        "draft_id"]
    await utils.db_delete_group_draft(draft_id, pgsql)
    response = await utils.groups_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_group_id" not in resp_json


async def test_modify_group_200(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    old_db_data = await utils.db_get_group(group_id, pgsql)
    response = await utils.modify_group(service_client, group_id, "test_group_1", False, access_token=access_token)
    resp_json = response.json()
    new_db_data = await utils.db_get_group(group_id, pgsql)
    assert response.status == 200
    GroupWithIdSchema(resp_json)
    assert old_db_data != new_db_data
    assert not resp_json["active"]


async def test_modify_group_401_missing_token(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    old_db_data = await utils.db_get_group(group_id, pgsql)
    response = await utils.modify_group(service_client, group_id, name, False)
    new_db_data = await utils.db_get_group(group_id, pgsql)
    resp_json = response.json()
    assert response.status == 401
    assert old_db_data == new_db_data
    assert "recipient_group_id" not in resp_json


async def test_modify_group_401_incorrect_user_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    await utils.delete_user(service_client, access_token)
    response = await utils.modify_group(service_client, group_id, name, False, access_token=access_token)
    resp_json = response.json()
    assert response.status == 401
    assert "recipient_group_id" not in resp_json


async def test_modify_group_404_non_uuid(service_client):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = "incorrect group_id"
    response = await utils.modify_group(service_client, group_id, name, active, access_token=access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_group_id" not in resp_json


async def test_modify_group_404_missing_group_id(service_client):
    name = "test_group_1"
    active = True
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = ""
    response = await utils.modify_group(service_client, group_id, name, active, access_token=access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_group_id" not in resp_json


async def test_modify_group_404_incorrect_group_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    await utils.delete_group(service_client, group_id, access_token)
    response = await utils.modify_group(service_client, group_id, name, False, access_token=access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_group_id" not in resp_json


async def test_modify_group_422_missing_property(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    old_db_data = await utils.db_get_group(group_id, pgsql)
    payload = {
        "name": name,
    }
    response = await service_client.put(
        '/groups/modifyGroup',
        data=json.dumps(payload),
        params={"group_id": group_id},
        headers={"Authorization": access_token}
    )
    new_db_data = await utils.db_get_group(group_id, pgsql)
    assert response.status == 422
    assert new_db_data == old_db_data
    assert "recipient_group_id" not in response.json()


async def test_modify_group_422_non_uuid_template_id(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    old_db_data = await utils.db_get_group(group_id, pgsql)
    payload = {
        "name": name,
        "active": False,
        "notification_template_id": "incorrect template_id",
    }
    response = await service_client.put(
        '/groups/modifyGroup',
        data=json.dumps(payload),
        params={"group_id": group_id},
        headers={"Authorization": access_token}
    )
    new_db_data = await utils.db_get_group(group_id, pgsql)
    assert response.status == 422
    assert new_db_data == old_db_data
    assert "recipient_group_id" not in response.json()


async def test_modify_group_422_incorrect_template_id(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    template_draft_id = (await utils.create_template(service_client, "test_template_1",
                                                     access_token=access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, template_draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_template(service_client, template_id, access_token)
    old_db_data = await utils.db_get_group(group_id, pgsql)
    payload = {
        "name": name,
        "active": False,
        "notification_template_id": template_id,
    }
    response = await service_client.put(
        '/groups/modifyGroup',
        data=json.dumps(payload),
        params={"group_id": group_id},
        headers={"Authorization": access_token}
    )
    new_db_data = await utils.db_get_group(group_id, pgsql)
    assert response.status == 422
    assert new_db_data == old_db_data
    assert "recipient_group_id" not in response.json()


async def test_group_add_recipient_200(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    db_group_recipients = await utils.db_get_group_recipients(group_id, 1, pgsql)
    assert response.status == 200
    assert len(db_group_recipients) == 1


async def test_group_add_recipient_401_missing_token(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id)
    db_group_recipients = await utils.db_get_group_recipients(group_id, 1, pgsql)
    assert response.status == 401
    assert len(db_group_recipients) == 0


async def test_group_add_recipient_401_incorrect_user_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id)
    assert response.status == 401


async def test_group_add_recipient_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = "incorrect group_id"
    recipient_id = "incorrect recipient_id"
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_group_add_recipient_404_missing_ids(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = ""
    recipient_id = ""
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_group_add_recipient_404_incorrect_recipient_id(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_recipient(service_client, recipient_id, access_token)
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    db_group_recipients = await utils.db_get_group_recipients(group_id, 1, pgsql)
    assert response.status == 404
    assert len(db_group_recipients) == 0


async def test_group_add_recipient_404_incorrect_group_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_group(service_client, group_id, access_token)
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_group_add_recipient_409_already_present(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 409


async def test_group_delete_recipient_200(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    db_group_recipients = await utils.db_get_group_recipients(group_id, 1, pgsql)
    assert response.status == 200
    assert len(db_group_recipients) == 0


async def test_group_delete_recipient_401_missing_token(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id)
    db_group_recipients = await utils.db_get_group_recipients(group_id, 1, pgsql)
    assert response.status == 401
    assert len(db_group_recipients) == 1


async def test_group_delete_recipient_401_incorrect_user_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    await utils.delete_user(service_client, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 401


async def test_group_delete_recipient_404_non_uuid(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_id = "incorrect recipient_id"
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_group_delete_recipient_404_missing_ids(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = ""
    recipient_id = ""
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_group_delete_recipient_404_incorrect_recipient_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.add_recipient_to_group(service_client, group_id, recipient_id, access_token)
    await utils.delete_recipient(service_client, recipient_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_group_delete_recipient_404_incorrect_group_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_group(service_client, group_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_group_delete_recipient_404_recipient_not_in_group(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    recipient_draft_id = (await utils.create_recipient(service_client, "recipient_1", "example@example.com",
                                                       access_token=access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, recipient_draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    response = await utils.delete_recipient_from_group(service_client, group_id, recipient_id, access_token)
    assert response.status == 404


async def test_delete_group_200(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    response = await utils.delete_group(service_client, group_id, access_token)
    db_groups = await utils.db_get_groups(1, pgsql)
    assert response.status == 200
    assert len(db_groups) == 0


async def test_delete_group_401_missing_token(service_client, pgsql):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    response = await utils.delete_group(service_client, group_id)
    db_groups = await utils.db_get_groups(1, pgsql)
    assert response.status == 401
    assert len(db_groups) == 1


async def test_delete_group_401_incorrect_user_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    await utils.delete_user(service_client, access_token)
    response = await utils.delete_group(service_client, group_id, access_token)
    assert response.status == 401


async def test_delete_group_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = "incorrect group_id"
    response = await utils.delete_group(service_client, group_id, access_token)
    assert response.status == 404


async def test_delete_group_404_missing_group_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    group_id = ""
    response = await utils.delete_group(service_client, group_id, access_token)
    assert response.status == 404


async def test_delete_group_404_incorrect_group_id(service_client):
    name = "test_group_1"
    active = True
    access_token, group_id = await create_group_w_confirmation(service_client, name, active)
    await utils.delete_group(service_client, group_id, access_token)
    response = await utils.delete_group(service_client, group_id, access_token)
    assert response.status == 404
