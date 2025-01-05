import json
import typing

import utils
from schemas import TemplateDraftSchema, TemplateWithIdSchema, TemplateWithIdListSchema


async def create_template_w_confirmation(service_client, name: str = "",
                                         message_text: str = "") -> typing.Tuple[str, str]:
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, name, message_text, access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    return access_token, template_id


async def test_create_template_200(service_client, pgsql):
    name = "test_template_1"
    message_text = "example_message"
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    response = await utils.create_template(service_client, name, message_text, access_token)
    resp_json = response.json()
    draft_id = resp_json["draft_id"]
    db_draft = await utils.db_get_template_draft(draft_id, pgsql)
    assert response.status == 200
    TemplateDraftSchema(resp_json)
    assert db_draft is not None


async def test_create_template_401_missing_token(service_client, pgsql):
    name = "test_template_1"
    message_text = "example_message"
    await utils.create_user("test_user_1", "1234", service_client)
    response = await utils.create_template(service_client, name, message_text)
    db_drafts = await utils.db_get_template_drafts(1, pgsql)
    assert response.status == 401
    assert len(db_drafts) == 0
    assert "draft_id" not in response.json()


async def test_create_template_401_incorrect_user_id(service_client, pgsql):
    name = "test_template_1"
    message_text = "example_message"
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    await utils.delete_user(service_client, access_token)
    response = await utils.create_template(service_client, name, message_text, access_token)
    db_drafts = await utils.db_get_template_drafts(1, pgsql)
    assert response.status == 401
    assert len(db_drafts) == 0
    assert "draft_id" not in response.json()


async def test_create_template_422(service_client, pgsql):
    name = "test_template_1"
    message_text = "example_message"
    payload = {
        "name": name,
        "message_text": message_text,
        "extra_property": "smth"
    }
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    response = await service_client.post(
        '/templates/create',
        data=json.dumps(payload),
        headers={"Authorization": access_token}
    )
    db_drafts = await utils.db_get_template_drafts(1, pgsql)
    assert response.status == 422
    assert len(db_drafts) == 0
    assert "draft_id" not in response.json()


async def test_get_template_200(service_client):
    name = "test_template_1"
    message_text = "example_message"
    access_token, template_id = await create_template_w_confirmation(service_client, name, message_text)
    response = await utils.get_template(service_client, template_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    TemplateWithIdSchema(resp_json)


async def test_get_template_401_missing_access_token(service_client):
    name = "test_template_1"
    message_text = "example_message"
    access_token, template_id = await create_template_w_confirmation(service_client, name, message_text)
    response = await utils.get_template(service_client, template_id)
    assert response.status == 401
    assert "template_id" not in response.json()


async def test_get_template_401_incorrect_user_id(service_client):
    name = "test_template_1"
    message_text = "example_message"
    access_token, template_id = await create_template_w_confirmation(service_client, name, message_text)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_template(service_client, template_id, access_token)
    assert response.status == 401
    assert "template_id" not in response.json()


async def test_get_template_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_id = "incorrect template_id"
    response = await utils.get_template(service_client, template_id, access_token)
    assert response.status == 404
    assert "template_id" not in response.json()


async def test_get_template_404_missing_template_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_id = ""
    response = await utils.get_template(service_client, template_id, access_token)
    assert response.status == 404
    assert "template_id" not in response.json()


async def test_get_template_404_incorrect_template_id(service_client):
    name = "test_template_1"
    message_text = "example_message"
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, name, message_text, access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_template(service_client, template_id, access_token)
    response = await utils.get_template(service_client, template_id, access_token)
    assert response.status == 404
    assert "template_id" not in response.json()


async def test_get_templates_200(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    for i in range(2):
        draft_id = (await utils.create_template(service_client, f"test_template_{i}", "message_text",
                                                access_token)).json()["draft_id"]
        await utils.templates_confirm_creation(service_client, draft_id, access_token)
    response = await utils.get_templates(service_client, access_token)
    resp_json = response.json()
    assert response.status == 200
    TemplateWithIdListSchema(resp_json)
    assert len(resp_json) == 2


async def test_get_templates_401_missing_access_token(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    for i in range(2):
        draft_id = (await utils.create_template(service_client, f"test_template_{i}", "message_text",
                                                access_token)).json()["draft_id"]
        await utils.templates_confirm_creation(service_client, draft_id, access_token)
    response = await utils.get_templates(service_client)
    assert response.status == 401


async def test_get_templates_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    draft_id_2 = (await utils.create_template(service_client, "test_template_2", "message_text", access_token)).json()[
        "draft_id"]
    await utils.templates_confirm_creation(service_client, draft_id_1, access_token)
    await utils.templates_confirm_creation(service_client, draft_id_2, access_token)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_templates(service_client)
    assert response.status == 401


async def test_confirm_template_creation_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    response = await utils.templates_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    template_id = resp_json["notification_template_id"]
    db_template = await utils.db_get_template(template_id, pgsql)
    assert response.status == 200
    TemplateWithIdSchema(resp_json)
    assert db_template is not None


async def test_confirm_template_creation_401_missing_token(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    response = await utils.templates_confirm_creation(service_client, draft_id)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 401
    assert len(db_templates) == 0


async def test_confirm_template_creation_401_incorrect_user_id(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.templates_confirm_creation(service_client, draft_id, access_token)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 401
    assert len(db_templates) == 0


async def test_confirm_template_creation_404_non_uuid(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = "incorrect_uuid"
    response = await utils.templates_confirm_creation(service_client, draft_id, access_token)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 404
    assert len(db_templates) == 0


async def test_confirm_template_creation_404_missing_draft_id(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = ""
    response = await utils.templates_confirm_creation(service_client, draft_id, access_token)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 404
    assert len(db_templates) == 0


async def test_confirm_template_creation_404_incorrect_draft_id(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    await utils.db_delete_template_draft(draft_id, pgsql)
    response = await utils.templates_confirm_creation(service_client, draft_id, access_token)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 404
    assert len(db_templates) == 0


async def test_modify_template_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    old_db_data = await utils.db_get_template(template_id, pgsql)
    response = await utils.modify_template(service_client, template_id, "test_template_1", "new_text", access_token)
    resp_json = response.json()
    new_db_data = await utils.db_get_template(template_id, pgsql)
    assert response.status == 200
    TemplateWithIdSchema(resp_json)
    assert old_db_data != new_db_data
    assert resp_json["message_text"] == "new_text"


async def test_modify_template_401_missing_token(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    old_db_data = await utils.db_get_template(template_id, pgsql)
    response = await utils.modify_template(service_client, template_id, "test_template_1", "new_text")
    new_db_data = await utils.db_get_template(template_id, pgsql)
    assert response.status == 401
    assert old_db_data == new_db_data
    assert "message_text" not in response.json()


async def test_modify_template_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.modify_template(service_client, template_id, "test_template_1", "new_text", access_token)
    assert response.status == 401
    assert "message_text" not in response.json()


async def test_modify_template_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_id = "incorrect template_id"
    response = await utils.modify_template(service_client, template_id, "test_template_1", "new_text", access_token)
    assert response.status == 404
    assert "message_text" not in response.json()


async def test_modify_template_404_missing_template_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    template_id = ""
    response = await utils.modify_template(service_client, template_id, "test_template_1", "new_text", access_token)
    assert response.status == 404
    assert "message_text" not in response.json()


async def test_modify_template_404_incorrect_template_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_template(service_client, template_id, access_token)
    response = await utils.modify_template(service_client, template_id, "test_template_1", "new_text", access_token)
    assert response.status == 404
    assert "message_text" not in response.json()


async def test_modify_template_422(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    old_db_data = await utils.db_get_template(template_id, pgsql)
    name = "new_name"
    message_text = "new_message"
    payload = {
        "name": name,
        "message_text": message_text,
        "extra_property": "smth"
    }
    response = await service_client.put(
        '/templates/modifyTemplate',
        data=json.dumps(payload),
        params={"template_id": template_id},
        headers={"Authorization": access_token}
    )
    new_db_data = await utils.db_get_template(template_id, pgsql)
    assert response.status == 422
    assert new_db_data == old_db_data
    assert "message_text" not in response.json()


async def test_delete_template_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    response = await utils.delete_template(service_client, template_id, access_token)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 200
    assert len(db_templates) == 0


async def test_delete_template_401_missing_token(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    response = await utils.delete_template(service_client, template_id)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 401
    assert len(db_templates) == 1


async def test_delete_template_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.delete_template(service_client, template_id, access_token)
    assert response.status == 401


async def test_delete_template_404_non_uuid(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    await utils.templates_confirm_creation(service_client, draft_id, access_token)
    template_id = "incorrect template_id"
    response = await utils.delete_template(service_client, template_id, access_token)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 404
    assert len(db_templates) == 1


async def test_delete_template_404_missing_template_id(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    await utils.templates_confirm_creation(service_client, draft_id, access_token)
    template_id = ""
    response = await utils.delete_template(service_client, template_id, access_token)
    db_templates = await utils.db_get_templates(1, pgsql)
    assert response.status == 404
    assert len(db_templates) == 1


async def test_delete_template_404_incorrect_template_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    await utils.delete_template(service_client, template_id, access_token)
    response = await utils.delete_template(service_client, template_id, access_token)
    assert response.status == 404
