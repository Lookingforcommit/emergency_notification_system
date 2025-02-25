import pathlib

import pytest
from testsuite.databases.pgsql import discover

pytest_plugins = ['pytest_userver.plugins.postgresql']

# TODO: get rid of secdist workaround
USERVER_CONFIG_HOOKS = ['prepare_service_config']
SERVICE_SOURCE_DIR = pathlib.Path(__file__).parent.parent


@pytest.fixture(scope="session")
def allowed_url_prefixes_extra():
    return ["https://api.telegram.org"]


@pytest.fixture(scope='session')
def service_source_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def prepare_service_config():
    def patch_config(config, config_vars):
        components = config['components_manager']['components']
        components['default-secdist-provider']['config'] = str(
            SERVICE_SOURCE_DIR / 'configs/secure_data.json',
        )

    return patch_config


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'ens',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))
