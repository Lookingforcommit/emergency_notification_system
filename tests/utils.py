import json

import typing

DB_NAME = "db_1"


def compact_dict(dct: dict) -> dict:
    return {k: v for k, v in dct.items() if v != ""}


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


async def db_get_recipient_drafts(rows_num: int, pgsql) -> typing.List[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient_draft "
    )
    records = cursor.fetchmany(rows_num)
    return records


async def db_delete_recipient_draft(draft_id: str, pgsql):
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "DELETE "
        "FROM ens_schema.recipient_draft "
        "WHERE recipient_draft_id = %s", (draft_id,),
    )


async def db_get_recipient(recipient_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient "
        "WHERE recipient_id = %s", (recipient_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_recipients(rows_num: int, pgsql) -> typing.List[tuple]:
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


async def db_get_template_drafts(rows_num: int, pgsql) -> typing.List[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.notification_template_draft "
    )
    records = cursor.fetchmany(rows_num)
    return records


async def db_delete_template_draft(draft_id: str, pgsql):
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "DELETE "
        "FROM ens_schema.notification_template_draft "
        "WHERE notification_template_draft_id = %s", (draft_id,),
    )


async def db_get_template(template_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.notification_template "
        "WHERE notification_template_id = %s", (template_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_templates(rows_num: int, pgsql) -> typing.List[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.notification_template "
    )
    records = cursor.fetchmany(rows_num)
    return records


async def create_group(service_client, name: str, active: bool, template_id: str = "", access_token: str = ""):
    payload = compact_dict({
        "name": name,
        "notification_template_id": template_id,
        "active": active
    })
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.post(
        '/groups/create',
        data=json.dumps(payload),
        headers=headers
    )
    return response


async def get_group(service_client, group_id: str, access_token: str = ""):
    params = {"group_id": group_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/groups',
        params=params,
        headers=headers
    )
    return response


async def get_group_recipients(service_client, group_id: str, access_token: str = ""):
    params = {"group_id": group_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/groups/recipients',
        params=params,
        headers=headers
    )
    return response


async def get_active_groups(service_client, access_token: str = ""):
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/groups/active',
        headers=headers
    )
    return response


async def get_groups(service_client, access_token: str = ""):
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.get(
        '/groups/all',
        headers=headers
    )
    return response


async def groups_confirm_creation(service_client, draft_id: str, access_token: str = ""):
    params = {"draft_id": draft_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/groups/confirmCreation',
        params=params,
        headers=headers,
    )
    return response


async def modify_group(service_client, group_id: str, name: str, active: bool, template_id: str = "",
                       access_token: str = ""):
    payload = compact_dict({
        "name": name,
        "notification_template_id": template_id,
        "active": active
    })
    params = {"group_id": group_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/groups/modifyGroup',
        params=params,
        headers=headers,
        data=json.dumps(payload)
    )
    return response


async def add_recipient_to_group(service_client, group_id: str, recipient_id: str, access_token: str = ""):
    params = {
        "group_id": group_id,
        "recipient_id": recipient_id
    }
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.put(
        '/groups/addRecipient',
        params=params,
        headers=headers,
    )
    return response


async def delete_recipient_from_group(service_client, group_id: str, recipient_id: str, access_token: str = ""):
    params = {
        "group_id": group_id,
        "recipient_id": recipient_id
    }
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.delete(
        '/groups/deleteRecipient',
        params=params,
        headers=headers,
    )
    return response


async def delete_group(service_client, group_id: str, access_token: str = ""):
    params = {"group_id": group_id}
    headers = compact_dict({"Authorization": access_token})
    response = await service_client.delete(
        '/groups/deleteGroup',
        params=params,
        headers=headers,
    )
    return response


async def db_get_group_draft(draft_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient_group_draft "
        "WHERE recipient_group_draft_id = %s", (draft_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_group_drafts(rows_num: int, pgsql) -> typing.List[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient_group_draft "
    )
    records = cursor.fetchmany(rows_num)
    return records


async def db_delete_group_draft(draft_id: str, pgsql):
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "DELETE "
        "FROM ens_schema.recipient_group_draft "
        "WHERE recipient_group_draft_id = %s", (draft_id,),
    )


async def db_get_group_recipients(group_id: str, rows_num: int, pgsql) -> typing.List[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient_recipient_group "
        "WHERE recipient_group_id = %s", (group_id,),
    )
    records = cursor.fetchmany(rows_num)
    return records


async def db_get_group(group_id: str, pgsql) -> typing.Optional[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient_group "
        "WHERE recipient_group_id = %s", (group_id,),
    )
    record = cursor.fetchone()
    return record


async def db_get_groups(rows_num: int, pgsql) -> typing.List[tuple]:
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute(
        "SELECT * "
        "FROM ens_schema.recipient_group "
    )
    records = cursor.fetchmany(rows_num)
    return records
