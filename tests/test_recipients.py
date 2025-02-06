import json
import typing

import utils
from schemas import RecipientDraftSchema, RecipientWithIdSchema, RecipientWithIdListSchema


async def create_recipient_w_confirmation(service_client, name: str = "", email: str = "", phone_number: str = "",
                                          telegram_id: str = "") -> typing.Tuple[str, str]:
    access_token = (await utils.create_user("test_user_1", "123456", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, name, email, phone_number, telegram_id,
                                             access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    return access_token, recipient_id


async def test_create_recipient_200(service_client, pgsql):
    name = "test_recipient_1"
    email = "example@domain.com"
    phone_number = "+1234567"
    telegram_id = 1111111111
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    response = await utils.create_recipient(service_client, name, email, phone_number, telegram_id, access_token)
    resp_json = response.json()
    draft_id = resp_json["draft_id"]
    db_draft = await utils.db_get_recipient_draft(draft_id, pgsql)
    assert response.status == 200
    RecipientDraftSchema(resp_json)
    assert db_draft is not None


async def test_create_recipient_401_missing_token(service_client, pgsql):
    name = "test_recipient_1"
    email = "test@example.com"
    phone_number = "+123456789"
    telegram_id = 1111111111
    await utils.create_user("test_user_1", "1234", service_client)
    response = await utils.create_recipient(service_client, name, email, phone_number, telegram_id)
    db_drafts = await utils.db_get_recipient_drafts(1, pgsql)
    assert response.status == 401
    assert len(db_drafts) == 0
    assert "recipient_id" not in response.json()


async def test_create_recipient_401_incorrect_user_id(service_client, pgsql):
    name = "test_recipient_1"
    email = "test@example.com"
    phone_number = "+123456789"
    telegram_id = 1111111111
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    await utils.delete_user(service_client, access_token)
    response = await utils.create_recipient(service_client, name, email, phone_number, telegram_id, access_token)
    db_drafts = await utils.db_get_recipient_drafts(1, pgsql)
    assert response.status == 401
    assert len(db_drafts) == 0
    assert "recipient_id" not in response.json()


async def test_create_recipient_422(service_client, pgsql):
    name = "test_recipient_1"
    email = "test@example.com"
    phone_number = "+123456789"
    telegram_id = 1111111111
    payload = {
        "name": name,
        "email": email,
        "phone_number": phone_number,
        "telegram_id": telegram_id,
        "extra_property": "smth"
    }
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    response = await service_client.post(
        '/recipients/create',
        data=json.dumps(payload),
        headers={"Authorization": access_token}
    )
    db_recipients = await utils.db_get_recipients(1, pgsql)
    assert response.status == 422
    assert len(db_recipients) == 0
    assert "recipient_id" not in response.json()


async def test_get_recipient_200(service_client):
    name = "test_recipient_1"
    email = "example@domain.com"
    phone_number = "+1234567"
    telegram_id = 1111111111
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, name, email, phone_number, telegram_id,
                                             access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.get_recipient(service_client, recipient_id, access_token)
    resp_json = response.json()
    assert response.status == 200
    RecipientWithIdSchema(resp_json)


async def test_get_recipient_401_missing_access_token(service_client):
    name = "test_recipient_1"
    email = "test@example.com"
    phone_number = "+123456789"
    telegram_id = 1111111111
    access_token, recipient_id = await create_recipient_w_confirmation(service_client, name, email, phone_number,
                                                                       telegram_id)
    response = await utils.get_recipient(service_client, recipient_id)
    assert response.status == 401
    assert "recipient_id" not in response.json()


async def test_get_recipient_401_incorrect_user_id(service_client):
    name = "test_recipient_1"
    email = "test@example.com"
    phone_number = "+123456789"
    telegram_id = 1111111111
    access_token, recipient_id = await create_recipient_w_confirmation(service_client, name, email, phone_number,
                                                                       telegram_id)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_recipient(service_client, recipient_id, access_token)
    assert response.status == 401
    assert "recipient_id" not in response.json()


async def test_get_recipient_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    recipient_id = "incorrect recipient_id"
    response = await utils.get_recipient(service_client, recipient_id, access_token)
    assert response.status == 404
    assert "recipient_id" not in response.json()


async def test_get_recipient_404_missing_recipient_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    recipient_id = ""
    response = await utils.get_recipient(service_client, recipient_id, access_token)
    assert response.status == 404
    assert "recipient_id" not in response.json()


async def test_get_recipient_404_incorrect_recipient_id(service_client):
    name = "test_recipient_1"
    email = "test@example.com"
    phone_number = "+123456789"
    telegram_id = 1111111111
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, name, email, phone_number, telegram_id,
                                             access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_recipient(service_client, recipient_id, access_token)
    response = await utils.get_recipient(service_client, recipient_id, access_token)
    assert response.status == 404
    assert "recipient_id" not in response.json()


async def test_get_recipients_200(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    for i in range(2):
        draft_id = (await utils.create_recipient(service_client, f"test_recipient_{i}", "example@domain.com",
                                                 "+1234567", 1111111111, access_token)).json()["draft_id"]
        await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    response = await utils.get_recipients(service_client, access_token)
    resp_json = response.json()
    assert response.status == 200
    RecipientWithIdListSchema(resp_json)
    assert len(resp_json) == 2


async def test_get_recipients_401_missing_token(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    for i in range(2):
        draft_id = (await utils.create_recipient(service_client, f"test_recipient_{i}", "example@domain.com",
                                                 "+1234567", 1111111111, access_token)).json()["draft_id"]
        await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    response = await utils.get_recipients(service_client)
    assert response.status == 401


async def test_get_recipients_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id_1 = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                               1111111111, access_token)).json()["draft_id"]
    draft_id_2 = (await utils.create_recipient(service_client, "test_recipient_2", "example@domain.com", "+1234567",
                                               1111111111, access_token)).json()["draft_id"]
    await utils.recipients_confirm_creation(service_client, draft_id_1, access_token)
    await utils.recipients_confirm_creation(service_client, draft_id_2, access_token)
    await utils.delete_user(service_client, access_token)
    response = await utils.get_recipients(service_client, access_token)
    assert response.status == 401


async def test_confirm_recipient_creation_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    response = await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    recipient_id = resp_json["recipient_id"]
    db_recipient = await utils.db_get_recipient(recipient_id, pgsql)
    assert response.status == 200
    RecipientWithIdSchema(resp_json)
    assert db_recipient is not None


async def test_confirm_recipient_creation_401_missing_token(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    response = await utils.recipients_confirm_creation(service_client, draft_id)
    resp_json = response.json()
    assert response.status == 401
    assert "recipient_id" not in resp_json


async def test_confirm_recipient_creation_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 401
    assert "recipient_id" not in resp_json


async def test_confirm_recipient_creation_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = "incorrect draft id"
    response = await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_id" not in resp_json


async def test_confirm_recipient_creation_404_missing_draft_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = ""
    response = await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_id" not in resp_json


async def test_confirm_recipient_creation_404_incorrect_draft_id(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    await utils.db_delete_recipient_draft(draft_id, pgsql)
    response = await utils.recipients_confirm_creation(service_client, draft_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "recipient_id" not in resp_json


async def test_modify_recipient_200(service_client, pgsql):
    name = "test_recipient_1"
    old_email = "example@domain.com"
    new_email = "newmail@domain.com"
    phone_number = "+1234567"
    old_telegram_id = 1111111111
    new_telegram_id = 2222222222
    access_token, recipient_id = await create_recipient_w_confirmation(service_client, name, old_email, phone_number,
                                                                       old_telegram_id)
    old_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    response = await utils.modify_recipient(service_client, recipient_id, name, new_email, phone_number,
                                            new_telegram_id, access_token)
    resp_json = response.json()
    new_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    assert response.status == 200
    RecipientWithIdSchema(resp_json)
    assert old_db_data != new_db_data
    assert resp_json["email"] == new_email and resp_json["telegram_id"] == new_telegram_id


async def test_modify_recipient_401_missing_token(service_client, pgsql):
    name = "test_recipient_1"
    old_email = "example@domain.com"
    new_email = "newmail@domain.com"
    phone_number = "+1234567"
    old_telegram_id = 1111111111
    new_telegram_id = 2222222222
    access_token, recipient_id = await create_recipient_w_confirmation(service_client, name, old_email, phone_number,
                                                                       old_telegram_id)
    old_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    response = await utils.modify_recipient(service_client, recipient_id, name, new_email, phone_number,
                                            new_telegram_id)
    new_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    resp_json = response.json()
    assert response.status == 401
    assert old_db_data == new_db_data
    assert "email" not in resp_json


async def test_modify_recipient_401_incorrect_user_id(service_client):
    name = "test_recipient_1"
    old_email = "example@domain.com"
    new_email = "newmail@domain.com"
    phone_number = "+1234567"
    old_telegram_id = 1111111111
    new_telegram_id = 2222222222
    access_token, recipient_id = await create_recipient_w_confirmation(service_client, name, old_email, phone_number,
                                                                       old_telegram_id)
    await utils.delete_user(service_client, access_token)
    response = await utils.modify_recipient(service_client, recipient_id, name, new_email, phone_number,
                                            new_telegram_id, access_token)
    resp_json = response.json()
    assert response.status == 401
    assert "email" not in resp_json


async def test_modify_recipient_404_non_uuid(service_client):
    name = "test_recipient_1"
    new_email = "newmail@domain.com"
    phone_number = "+1234567"
    new_telegram_id = 1111111111
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    recipient_id = "incorrect recipient_id"
    response = await utils.modify_recipient(service_client, recipient_id, name, new_email, phone_number,
                                            new_telegram_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "email" not in resp_json


async def test_modify_recipient_404_missing_recipient_id(service_client):
    name = "test_recipient_1"
    new_email = "newmail@domain.com"
    phone_number = "+1234567"
    new_telegram_id = 1111111111
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    recipient_id = ""
    response = await utils.modify_recipient(service_client, recipient_id, name, new_email, phone_number,
                                            new_telegram_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "email" not in resp_json


async def test_modify_recipient_404_incorrect_recipient_id(service_client):
    name = "test_recipient_1"
    old_email = "example@domain.com"
    new_email = "newmail@domain.com"
    phone_number = "+1234567"
    old_telegram_id = 1111111111
    new_telegram_id = 2222222222
    access_token, recipient_id = await create_recipient_w_confirmation(service_client, name, old_email, phone_number,
                                                                       old_telegram_id)
    await utils.delete_recipient(service_client, recipient_id, access_token)
    response = await utils.modify_recipient(service_client, recipient_id, name, new_email, phone_number,
                                            new_telegram_id, access_token)
    resp_json = response.json()
    assert response.status == 404
    assert "email" not in resp_json


async def test_modify_recipient_422(service_client, pgsql):
    name = "test_recipient_1"
    old_email = "example@domain.com"
    new_email = "newmail@domain.com"
    phone_number = "+1234567"
    old_telegram_id = 111111111
    new_telegram_id = 222222222
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, name, old_email, phone_number, old_telegram_id,
                                             access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    old_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    payload = {
        "name": name,
        "email": new_email,
        "telegram_id": new_telegram_id,
        "phone_number": phone_number,
        "extra_property": "smth"
    }
    response = await service_client.put(
        '/recipients/modifyRecipient',
        data=json.dumps(payload),
        params={"recipient_id": recipient_id},
        headers={"Authorization": access_token}
    )
    new_db_data = await utils.db_get_recipient(recipient_id, pgsql)
    assert response.status == 422
    assert new_db_data == old_db_data
    assert "email" not in response.json()


async def test_delete_recipient_200(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.delete_recipient(service_client, recipient_id, access_token)
    db_recipients = await utils.db_get_recipients(1, pgsql)
    assert response.status == 200
    assert len(db_recipients) == 0


async def test_delete_recipient_401_missing_token(service_client, pgsql):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    response = await utils.delete_recipient(service_client, recipient_id)
    db_recipients = await utils.db_get_recipients(1, pgsql)
    assert response.status == 401
    assert len(db_recipients) == 1


async def test_delete_recipient_401_incorrect_user_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_user(service_client, access_token)
    response = await utils.delete_recipient(service_client, recipient_id, access_token)
    assert response.status == 401


async def test_delete_recipient_404_non_uuid(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    recipient_id = "incorrect recipient_id"
    response = await utils.delete_recipient(service_client, recipient_id, access_token)
    assert response.status == 404


async def test_delete_recipient_404_missing_recipient_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    recipient_id = ""
    response = await utils.delete_recipient(service_client, recipient_id, access_token)
    assert response.status == 404


async def test_delete_recipient_404_incorrect_recipient_id(service_client):
    access_token = (await utils.create_user("test_user_1", "1234", service_client)).json()["access_token"]
    draft_id = (await utils.create_recipient(service_client, "test_recipient_1", "example@domain.com", "+1234567",
                                             1111111111, access_token)).json()["draft_id"]
    recipient_id = (await utils.recipients_confirm_creation(service_client, draft_id, access_token)).json()[
        "recipient_id"]
    await utils.delete_recipient(service_client, recipient_id, access_token)
    response = await utils.delete_recipient(service_client, recipient_id, access_token)
    assert response.status == 404
