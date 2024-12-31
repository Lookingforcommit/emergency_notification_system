-- TODO: Add tests for values exceeding VARCHAR length

DROP SCHEMA IF EXISTS ens_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS ens_schema;

DROP TABLE IF EXISTS ens_schema.user CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.user
(
    name          VARCHAR(256) UNIQUE NOT NULL,
    password_hash TEXT                NOT NULL,
    password_salt TEXT                NOT NULL,
    user_id       uuid PRIMARY KEY
);

DROP TABLE IF EXISTS ens_schema.notification_template CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.notification_template
(
    name                     VARCHAR(64) NOT NULL,
    message_text             VARCHAR(1024),
    notification_template_id uuid PRIMARY KEY,
    master_id                uuid        NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id) ON DELETE CASCADE
);

DROP TABLE IF EXISTS ens_schema.notification_template_draft CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.notification_template_draft
(
    name                           VARCHAR(64) NOT NULL,
    message_text                   VARCHAR(1024),
    notification_template_draft_id uuid PRIMARY KEY,
    master_id                      uuid        NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id) ON DELETE CASCADE
);

DROP TABLE IF EXISTS ens_schema.recipient CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient
(
    name              VARCHAR(64) NOT NULL,
    email             VARCHAR(512),
    phone_number      VARCHAR(32),
    telegram_username VARCHAR(128),
    recipient_id      uuid PRIMARY KEY,
    master_id         uuid        NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id) ON DELETE CASCADE
);

DROP TABLE IF EXISTS ens_schema.recipient_draft CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_draft
(
    name               VARCHAR(64) NOT NULL,
    email              VARCHAR(512),
    phone_number       VARCHAR(32),
    telegram_username  VARCHAR(128),
    master_id          uuid        NOT NULL,
    recipient_draft_id uuid PRIMARY KEY,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id) ON DELETE CASCADE
);

DROP TABLE IF EXISTS ens_schema.recipient_group CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_group
(
    name               VARCHAR(64) NOT NULL,
    active             BOOLEAN     NOT NULL,
    recipient_group_id uuid PRIMARY KEY,
    master_id          uuid        NOT NULL,
    template_id        uuid,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id) ON DELETE CASCADE,
    FOREIGN KEY (template_id) REFERENCES ens_schema.notification_template (notification_template_id) ON DELETE CASCADE
);

DROP TABLE IF EXISTS ens_schema.recipient_group_draft CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_group_draft
(
    name                     VARCHAR(64) NOT NULL,
    active                   BOOLEAN     NOT NULL,
    recipient_group_draft_id uuid PRIMARY KEY,
    master_id                uuid        NOT NULL,
    template_id              uuid,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id) ON DELETE CASCADE,
    FOREIGN KEY (template_id) REFERENCES ens_schema.notification_template (notification_template_id) ON DELETE CASCADE
);

DROP TABLE IF EXISTS ens_schema.recipient_recipient_group CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_recipient_group
(
    recipient_id       uuid NOT NULL,
    recipient_group_id uuid NOT NULL,
    PRIMARY KEY (recipient_id, recipient_group_id),
    FOREIGN KEY (recipient_id) REFERENCES ens_schema.recipient (recipient_id) ON DELETE CASCADE,
    FOREIGN KEY (recipient_group_id) REFERENCES ens_schema.recipient_group (recipient_group_id) ON DELETE CASCADE
);

DROP TABLE IF EXISTS ens_schema.notification CASCADE;

DROP TYPE IF EXISTS ens_schema.message_type;

CREATE TYPE ens_schema.message_type AS ENUM ('Telegram', 'SMS', 'Mail');

CREATE TABLE IF NOT EXISTS ens_schema.notification
(
    type                 ens_schema.message_type NOT NULL,
    creation_timestamp   TIMESTAMP               NOT NULL,
    completion_timestamp TIMESTAMP,
    notification_id      uuid PRIMARY KEY,
    master_id            uuid                    NOT NULL,
    recipient_id         uuid                    NOT NULL,
    group_id             uuid                    NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id) ON DELETE CASCADE,
    FOREIGN KEY (recipient_id) REFERENCES ens_schema.recipient (recipient_id) ON DELETE CASCADE,
    FOREIGN KEY (group_id) REFERENCES ens_schema.recipient_group (recipient_group_id) ON DELETE CASCADE
);
