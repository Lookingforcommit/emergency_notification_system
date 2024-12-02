from voluptuous import Schema, Required, Optional

import utils

BaseRecipientSchema = Schema({
    Required("name"): str,
    Optional("email"): str,
    Optional("phone_number"): str,
    Optional("telegram_username"): str,
})

ReturnedRecipientSchema = BaseRecipientSchema.extend({
    Required("master_id"): str
})

RecipientWithoutIdSchema = BaseRecipientSchema

RecipientDraftSchema = ReturnedRecipientSchema.extend({
    Required("draft_id"): str
})

RecipientWithIdSchema = ReturnedRecipientSchema.extend({
    Required("recipient_id"): str
})

RecipientWithIdListSchema = Schema([RecipientWithIdSchema])


async def test_create_recipient_200(service_client, pgsql):
    name = "test_recipient_1"
    email = "example@domain.com"
    phone_number = "+1234567"
    telegram_username = "@example_username"
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    response = await utils.create_recipient(service_client, name, email, phone_number, telegram_username, access_token)
    resp_json = response.json()
    draft_id = resp_json["draft_id"]
    db_draft = await utils.db_get_recipient_draft(draft_id, pgsql)
    assert response.status == 200
    RecipientDraftSchema(resp_json)
    assert db_draft is not None


async def test_get_recipient_200(service_client, pgsql):
    name = "test_recipient_1"
    email = "example@domain.com"
    phone_number = "+1234567"
    telegram_username = "@example_username"
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, name, email, phone_number, telegram_username,
                                             access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.get_recipient(service_client, recipient_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    RecipientWithIdSchema(resp_json)


async def test_get_recipients_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                               "@example_username", access_token)).json()["draft_id"]
    draft_id_2 = (await utils.create_recipient(service_client, "test_recipient_2", "example@domain.com", "+1234567",
                                               "@example_username", access_token)).json()["draft_id"]
    await utils.recipients_confirm_creation(service_client, draft_id_1, access_token)
    await utils.recipients_confirm_creation(service_client, draft_id_2, access_token)
    response = await utils.get_recipients(service_client, access_token)
    resp_json = response.json()
    assert response.status == 200
    RecipientWithIdListSchema(resp_json)
    assert len(resp_json) == 2


async def test_confirm_recipient_creation_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             "@example_username", access_token)).json()["draft_id"]
    response = await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    recipient_id = resp_json["recipient_id"]
    db_recipient = await utils.db_get_recipient(recipient_id, pgsql)
    assert response.status == 200
    RecipientWithIdSchema(resp_json)
    assert db_recipient is not None


async def test_modify_recipient_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             "@example_username", access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    old_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    response = await utils.modify_recipient(service_client, recipient_id, "test_recipient_1", "newmail@domain.com",
                                            "+1234567", "@new_username", access_token)
    resp_json = response.json()
    new_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    assert response.status == 200
    RecipientWithIdSchema(resp_json)
    assert old_db_data != new_db_data
    assert resp_json["email"] == "newmail@domain.com" and resp_json["telegram_username"] == "@new_username"


async def test_delete_recipient_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             "@example_username", access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.delete_recipient(service_client, recipient_id, access_token)
    db_recipients = await utils.db_get_recipients(1, pgsql)
    assert response.status == 200
    assert len(db_recipients) == 0
