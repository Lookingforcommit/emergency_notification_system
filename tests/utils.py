import json

import typing

DB_NAME = "db_1"


def compact_dict(dct: dict) -> dict:
    return {k: v for k, v in dct.items() if v}


async def create_user(name: str, password: str, service_client):
    payload = {
        "name": name,
        "password": password,
    }
    response = await service_client.post(
        '/user/create',
        data=json.dumps(payload),
    )
    return response


async def login_user(name: str, password: str, service_client):
    payload = {
        "name": name,
        "password": password,
    }
    response = await service_client.post(
        '/user/login',
        data=json.dumps(payload),
    )
    return response


async def modify_user(new_name: str, new_password: str, service_client, access_token: str = ""):
    payload = {
        "name": new_name,
        "password": new_password,
    }
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/user/modifyUser',
        data=json.dumps(payload),
        headers=headers,
    )
    return response


async def delete_user(service_client, access_token: str = ""):
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.delete(
        '/user',
        headers=headers,
    )
    return response


async def db_get_user_by_name(name: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.user "
        "WHERE name = %s", (name,),
    )
    record = cursor.fetchone()
    return record


async def db_get_users(rows_num: int, pgsql) -> typing.List[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.user"
    )
    records = cursor.fetchmany(rows_num)
    return records


async def create_recipient(service_client, name: str, email: str = "", phone_number: str = "",
                           telegram_username: str = "", access_token: str = ""):
    payload = compact_dict({
        "name": name,
        "email": email,
        "phone_number": phone_number,
        "telegram_username": telegram_username
    })
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.post(
        '/recipients/create',
        data=json.dumps(payload),
        headers=headers
    )
    return response


async def get_recipient(service_client, recipient_id: str, access_token: str = ""):
    params = {"recipient_id": recipient_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/recipients',
        params=params,
        headers=headers
    )
    return response


async def get_recipients(service_client, access_token: str = ""):
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/recipients/all',
        headers=headers
    )
    return response


async def recipients_confirm_creation(service_client, draft_id: str, access_token: str = ""):
    params = {"draft_id": draft_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/recipients/confirmCreation',
        params=params,
        headers=headers,
    )
    return response


async def modify_recipient(service_client, recipient_id: str, name: str, email: str = "", phone_number: str = "",
                           telegram_username: str = "", access_token: str = ""):
    payload = compact_dict({
        "name": name,
        "email": email,
        "phone_number": phone_number,
        "telegram_username": telegram_username
    })
    params = {"recipient_id": recipient_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/recipients/modifyRecipient',
        params=params,
        headers=headers,
        data=json.dumps(payload)
    )
    return response


async def delete_recipient(service_client, recipient_id: str, access_token: str = ""):
    params = {"recipient_id": recipient_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.delete(
        '/recipients/deleteRecipient',
        params=params,
        headers=headers,
    )
    return response


async def db_get_recipient_draft(draft_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient_draft "
        "WHERE recipient_draft_id = %s", (draft_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_recipient(recipient_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient "
        "WHERE recipient_id = %s", (recipient_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_recipients(rows_num: int, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient "
    )
    records = cursor.fetchmany(rows_num)
    return records


async def create_template(service_client, name: str, message_text: str = "", access_token: str = ""):
    payload = compact_dict({
        "name": name,
        "message_text": message_text,
    })
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.post(
        '/templates/create',
        data=json.dumps(payload),
        headers=headers
    )
    return response


async def get_template(service_client, template_id: str, access_token: str = ""):
    params = {"template_id": template_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/templates',
        params=params,
        headers=headers
    )
    return response


async def get_templates(service_client, access_token: str = ""):
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/templates/all',
        headers=headers
    )
    return response


async def templates_confirm_creation(service_client, draft_id: str, access_token: str = ""):
    params = {"draft_id": draft_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/templates/confirmCreation',
        params=params,
        headers=headers,
    )
    return response


async def modify_template(service_client, template_id: str, name: str, message_text: str = "", access_token: str = ""):
    payload = compact_dict({
        "name": name,
        "message_text": message_text
    })
    params = {"template_id": template_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/templates/modifyTemplate',
        params=params,
        headers=headers,
        data=json.dumps(payload)
    )
    return response


async def delete_template(service_client, template_id: str, access_token: str = ""):
    params = {"template_id": template_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.delete(
        '/templates/deleteTemplate',
        params=params,
        headers=headers,
    )
    return response


async def db_get_template_draft(draft_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.notification_template_draft "
        "WHERE notification_template_draft_id = %s", (draft_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_template(template_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.notification_template "
        "WHERE notification_template_id = %s", (template_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_templates(rows_num: int, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.notification_template "
    )
    records = cursor.fetchmany(rows_num)
    return records
