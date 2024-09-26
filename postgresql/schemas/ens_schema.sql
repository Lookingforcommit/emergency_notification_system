DROP SCHEMA IF EXISTS ens_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS ens_schema;

DROP TABLE IF EXISTS ens_schema.user CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.user
(
    name          TEXT NOT NULL,
    password_hash TEXT NOT NULL,
    user_id       TEXT PRIMARY KEY
);

DROP TABLE IF EXISTS ens_schema.notification_template CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.notification_template
(
    name                     TEXT NOT NULL,
    message_text             TEXT,
    notification_template_id TEXT PRIMARY KEY,
    master_id                TEXT NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id)
);

DROP TABLE IF EXISTS ens_schema.notification_template_draft CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.notification_template_draft
(
    master_id                      TEXT NOT NULL,
    name                           TEXT NOT NULL,
    message_text                   TEXT,
    notification_template_draft_id TEXT PRIMARY KEY
);

DROP TABLE IF EXISTS ens_schema.recipient CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient
(
    name              TEXT NOT NULL,
    email             TEXT,
    phone_number      TEXT,
    telegram_username TEXT,
    recipient_id      TEXT PRIMARY KEY,
    master_id         TEXT NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id)
);

DROP TABLE IF EXISTS ens_schema.recipient_draft CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_draft
(
    master_id          TEXT NOT NULL,
    name               TEXT NOT NULL,
    email              TEXT,
    phone_number       TEXT,
    telegram_username  TEXT,
    recipient_draft_id TEXT PRIMARY KEY
);

DROP TABLE IF EXISTS ens_schema.recipient_group CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_group
(
    name               TEXT    NOT NULL,
    active             BOOLEAN NOT NULL,
    recipient_group_id TEXT PRIMARY KEY,
    master_id          TEXT    NOT NULL,
    template_id        TEXT    NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id),
    FOREIGN KEY (template_id) REFERENCES ens_schema.notification_template (notification_template_id)
);

DROP TABLE IF EXISTS ens_schema.recipient_group_draft CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_group_draft
(
    master_id                TEXT    NOT NULL,
    template_id              TEXT    NOT NULL,
    name                     TEXT    NOT NULL,
    active                   BOOLEAN NOT NULL,
    recipient_group_draft_id TEXT PRIMARY KEY
);

DROP TABLE IF EXISTS ens_schema.recipient_recipient_group CASCADE;

CREATE TABLE IF NOT EXISTS ens_schema.recipient_recipient_group
(
    recipient_recipient_group TEXT PRIMARY KEY,
    recipient_id              TEXT NOT NULL,
    recipient_group_id        TEXT NOT NULL,
    FOREIGN KEY (recipient_id) REFERENCES ens_schema.recipient (recipient_id),
    FOREIGN KEY (recipient_group_id) REFERENCES ens_schema.recipient_group (recipient_group_id)
);

DROP TABLE IF EXISTS ens_schema.notification CASCADE;

DROP TYPE IF EXISTS message_type;

CREATE TYPE message_type AS ENUM ('Telegram', 'SMS', 'Mail');

CREATE TABLE IF NOT EXISTS ens_schema.notification
(
    type                 message_type NOT NULL,
    creation_timestamp   TIMESTAMP    NOT NULL,
    completion_timestamp TIMESTAMP,
    notification_id      TEXT PRIMARY KEY,
    master_id            TEXT         NOT NULL,
    recipient_id         TEXT         NOT NULL,
    group_id             TEXT         NOT NULL,
    FOREIGN KEY (master_id) REFERENCES ens_schema.user (user_id),
    FOREIGN KEY (recipient_id) REFERENCES ens_schema.recipient (recipient_id),
    FOREIGN KEY (group_id) REFERENCES ens_schema.recipient_group (recipient_group_id)
);
