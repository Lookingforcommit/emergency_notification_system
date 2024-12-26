import json

import utils
from schemas import JwtSchema


async def test_create_user_200(service_client, pgsql):
    name = "test_user_1"
    password = "1234"
    response = await utils.create_user(name, password, service_client)
    resp_json = response.json()
    db_user = await utils.db_get_user_by_name(name, pgsql)
    assert response.status == 200
    JwtSchema(resp_json)
    assert db_user is not None


async def test_create_user_409(service_client):
    name = "test_user_1"
    password = "1234"
    await utils.create_user(name, password, service_client)
    response = await utils.create_user(name, password, service_client)
    assert response.status == 409
    assert "access_token" not in response.json()


async def test_create_user_422(service_client, pgsql):
    name = "test_user_1"
    payload = {
        "name": name,
        "extra_property": "smth"
    }
    response = await service_client.post(
        '/user/create',
        data=json.dumps(payload),
    )
    resp_json = response.json()
    db_user = await utils.db_get_user_by_name(name, pgsql)
    assert response.status == 422
    assert "access_token" not in resp_json
    assert db_user is None


async def test_login_user_200(service_client):
    name = "test_user_1"
    password = "1234"
    await utils.create_user(name, password, service_client)
    response = await utils.login_user(name, password, service_client)
    resp_json = response.json()
    assert response.status == 200
    JwtSchema(resp_json)


async def test_login_user_401(service_client):
    name = "test_user_1"
    password = "1234"
    incorrect_password = "incorrect_password"
    await utils.create_user(name, password, service_client)
    response = await utils.login_user(name, incorrect_password, service_client)
    assert response.status == 401
    assert "access_token" not in response.json()


async def test_login_user_404(service_client):
    name = "test_user_1"
    password = "1234"
    response = await utils.login_user(name, password, service_client)
    assert response.status == 404


async def test_login_user_422(service_client):
    name = "test_user_1"
    password = "1234"
    payload = {
        "extra_property": "smth",
        "password": password,
    }
    response = await service_client.post(
        '/user/login',
        data=json.dumps(payload),
    )
    await utils.create_user(name, password, service_client)
    resp_json = response.json()
    assert response.status == 422
    assert "access_token" not in resp_json


async def test_modify_user_200(service_client, pgsql):
    name = "test_user_1"
    password = "1234"
    auth_response = await utils.create_user(name, password, service_client)
    access_token = auth_response.json()["access_token"]
    old_db_data = await utils.db_get_user_by_name(name, pgsql)
    new_password = "new_password"
    response = await utils.modify_user(name, new_password, service_client, access_token)
    new_db_data = await utils.db_get_user_by_name(name, pgsql)
    assert response.status == 200
    assert new_db_data != old_db_data


async def test_modify_user_401(service_client, pgsql):
    name = "test_user_1"
    password = "1234"
    await utils.create_user(name, password, service_client)
    old_db_data = await utils.db_get_user_by_name(name, pgsql)
    new_password = "new_password"
    response = await utils.modify_user(name, new_password, service_client)
    new_db_data = await utils.db_get_user_by_name(name, pgsql)
    assert response.status == 401
    assert new_db_data == old_db_data


async def test_modify_user_422(service_client, pgsql):
    name = "test_user_1"
    password = "1234"
    auth_response = await utils.create_user(name, password, service_client)
    access_token = auth_response.json()["access_token"]
    old_db_data = await utils.db_get_user_by_name(name, pgsql)
    payload = {
        "name": name,
        "password": password,
        "extra_property": "smth"
    }
    response = await service_client.put(
        '/user/modifyUser',
        data=json.dumps(payload),
        headers={"Authorization": access_token},
    )
    new_db_data = await utils.db_get_user_by_name(name, pgsql)
    assert response.status == 422
    assert new_db_data == old_db_data


async def test_refresh_token_200(service_client):
    name = "test_user_1"
    password = "1234"
    auth_response = await utils.create_user(name, password, service_client)
    refresh_token = auth_response.json()["refresh_token"]
    response = await service_client.put(
        '/user/refreshToken',
        headers={"Authorization": refresh_token},
    )
    resp_json = response.json()
    assert response.status == 200
    JwtSchema(resp_json)


async def test_refresh_token_404(service_client):
    name = "test_user_1"
    password = "1234"
    await utils.create_user(name, password, service_client)
    refresh_token = "incorrect_refresh_token"
    response = await service_client.put(
        '/user/refreshToken',
        headers={"Authorization": refresh_token},
    )
    assert response.status == 404
    assert "access_token" not in response.json()


async def test_delete_user_200(service_client, pgsql):
    name = "test_user_1"
    password = "1234"
    auth_response = await utils.create_user(name, password, service_client)
    access_token = auth_response.json()["access_token"]
    response = await utils.delete_user(service_client, access_token)
    db_users = await utils.db_get_users(1, pgsql)
    assert response.status == 200
    assert len(db_users) == 0


async def test_delete_user_401(service_client, pgsql):
    name = "test_user_1"
    password = "1234"
    await utils.create_user(name, password, service_client)
    response = await utils.delete_user(service_client)
    db_users = await utils.db_get_users(1, pgsql)
    assert response.status == 401
    assert len(db_users) == 1
