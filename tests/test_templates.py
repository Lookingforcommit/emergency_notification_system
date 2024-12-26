import utils
from schemas import TemplateDraftSchema, TemplateWithIdSchema, TemplateWithIdListSchema


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


async def test_get_template_200(service_client, pgsql):
    name = "test_template_1"
    message_text = "example_message"
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_template(service_client, name, message_text, access_token)).json()["draft_id"]
    template_id = (await utils.templates_confirm_creation(service_client, draft_id, access_token)).json()[
        "notification_template_id"]
    response = await utils.get_template(service_client, template_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    TemplateWithIdSchema(resp_json)


async def test_get_templates_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_template(service_client, "test_template_1", "message_text", access_token)).json()[
        "draft_id"]
    draft_id_2 = (await utils.create_template(service_client, "test_template_2", "message_text", access_token)).json()[
        "draft_id"]
    await utils.templates_confirm_creation(service_client, draft_id_1, access_token)
    await utils.templates_confirm_creation(service_client, draft_id_2, access_token)
    response = await utils.get_templates(service_client, access_token)
    resp_json = response.json()
    assert response.status == 200
    TemplateWithIdListSchema(resp_json)
    assert len(resp_json) == 2


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
