#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>

#include <userver/chaotic/type_bundle_hpp.hpp>

namespace schemas {

// Base schema for all NotificationTemplate schemas
struct BaseNotificationTemplate {
  std::string name{};
  std::optional<std::string> message_text{};

  BaseNotificationTemplate() = default;

  BaseNotificationTemplate(const std::string &name, const std::optional<std::string> &message_text)
  : name(name),
    message_text(message_text) {};
};

bool operator==(const schemas::BaseNotificationTemplate &lhs,
                const schemas::BaseNotificationTemplate &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::BaseNotificationTemplate &value);

BaseNotificationTemplate Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseNotificationTemplate>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::BaseNotificationTemplate &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// Base schema for all Recipient schemas
struct BaseRecipient {
  std::string name{};
  std::optional<std::string> email{};
  std::optional<std::string> phone_number{};
  std::optional<std::string> telegram_username{};
  BaseRecipient(const std::string &name, const std::optional<std::string> &email,
                const std::optional<std::string> &phone_number, const std::optional<std::string> &telegram_username)
      : name(name),
        email(email),
        phone_number(phone_number),
        telegram_username(telegram_username) {}
  BaseRecipient() = default;
};

bool operator==(const schemas::BaseRecipient &lhs,
                const schemas::BaseRecipient &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::BaseRecipient &value);

BaseRecipient Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseRecipient>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::BaseRecipient &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// Base schema for all RecipientGroup schemas
struct BaseRecipientGroup {
  std::string name{};
  std::optional<std::string> notification_template_id{};
  bool active{};
};

bool operator==(const schemas::BaseRecipientGroup &lhs,
                const schemas::BaseRecipientGroup &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::BaseRecipientGroup &value);

BaseRecipientGroup Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseRecipientGroup>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::BaseRecipientGroup &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

using JWTString = std::string;

// Notification sent by user
struct Notification {
  enum class Type {
    kTelegram,
    kSms,
    kMail,
  };

  static constexpr Type kTypeValues[] = {
      Type::kTelegram,
      Type::kSms,
      Type::kMail,
  };

  std::string master_id{};
  std::string recipient_id{};
  std::string group_id{};
  schemas::Notification::Type type{};
  std::string creation_timestamp{};
  std::string completion_timestamp{};
};

bool operator==(const schemas::Notification &lhs,
                const schemas::Notification &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::Notification::Type &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::Notification &value);

Notification::Type Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification::Type>);

Notification Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification>);

Notification::Type FromString(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification::Type>);

Notification::Type Parse(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification::Type>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::Notification::Type &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::Notification &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

std::string ToString(schemas::Notification::Type value);

using NotificationList = std::vector<schemas::Notification>;

// Base schema for all NotificationTemplate schemas returned by the server
struct ReturnedNotificationTemplate_P1 {
  std::string master_id{};

  USERVER_NAMESPACE::formats::json::Value extra;

  ReturnedNotificationTemplate_P1() = default;

  ReturnedNotificationTemplate_P1(const std::string &master_id) : master_id(master_id) {};
};

struct ReturnedNotificationTemplate
    : public schemas::BaseNotificationTemplate,
      public schemas::ReturnedNotificationTemplate_P1 {
  ReturnedNotificationTemplate() = default;

  ReturnedNotificationTemplate(schemas::BaseNotificationTemplate &&a0,
                               schemas::ReturnedNotificationTemplate_P1 &&a1)
      : schemas::BaseNotificationTemplate(std::move(a0)),
        schemas::ReturnedNotificationTemplate_P1(std::move(a1)) {};

  ReturnedNotificationTemplate(const std::string &master_id, const std::string &name,
                               const std::optional<std::string> &message_text)
      : BaseNotificationTemplate(name, message_text),
        ReturnedNotificationTemplate_P1(master_id) {};
};

bool operator==(const schemas::ReturnedNotificationTemplate_P1 &lhs,
                const schemas::ReturnedNotificationTemplate_P1 &rhs);

bool operator==(const schemas::ReturnedNotificationTemplate &lhs,
                const schemas::ReturnedNotificationTemplate &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedNotificationTemplate_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedNotificationTemplate &value);

ReturnedNotificationTemplate_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::ReturnedNotificationTemplate_P1>);

ReturnedNotificationTemplate Parse(USERVER_NAMESPACE::formats::json::Value json,
                                   USERVER_NAMESPACE::formats::parse::To<
                                       schemas::ReturnedNotificationTemplate>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedNotificationTemplate_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedNotificationTemplate &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// NotificationTemplate draft object. Used to represent a NotificationTemplate
// object pending for creation
struct NotificationTemplateDraft_P1 {
  std::string draft_id{};

  USERVER_NAMESPACE::formats::json::Value extra;

  NotificationTemplateDraft_P1() = default;

  NotificationTemplateDraft_P1(const std::string &draft_id) : draft_id(draft_id) {};
};

struct NotificationTemplateDraft
    : public schemas::ReturnedNotificationTemplate,
      public schemas::NotificationTemplateDraft_P1 {
  NotificationTemplateDraft() = default;

  NotificationTemplateDraft(schemas::ReturnedNotificationTemplate &&a0,
                            schemas::NotificationTemplateDraft_P1 &&a1)
      : schemas::ReturnedNotificationTemplate(std::move(a0)),
        schemas::NotificationTemplateDraft_P1(std::move(a1)) {};

  NotificationTemplateDraft(const std::string &draft_id, const std::string &master_id, const std::string &name,
                            const std::optional<std::string> &message_text)
                            : ReturnedNotificationTemplate(master_id, name, message_text),
                              NotificationTemplateDraft_P1(draft_id) {};
};

bool operator==(const schemas::NotificationTemplateDraft_P1 &lhs,
                const schemas::NotificationTemplateDraft_P1 &rhs);

bool operator==(const schemas::NotificationTemplateDraft &lhs,
                const schemas::NotificationTemplateDraft &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateDraft_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateDraft &value);

NotificationTemplateDraft_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateDraft_P1>);

NotificationTemplateDraft Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::NotificationTemplateDraft>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateDraft_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateDraft &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// NotificationTemplate object with notificationTemplateId. Used to represent
// server-generated objects
struct NotificationTemplateWithId_P1 {
  std::string notification_template_id{};

  USERVER_NAMESPACE::formats::json::Value extra;

  NotificationTemplateWithId_P1() = default;

  NotificationTemplateWithId_P1(const std::string &notification_template_id)
  : notification_template_id(notification_template_id) {};
};

struct NotificationTemplateWithId
    : public schemas::ReturnedNotificationTemplate,
      public schemas::NotificationTemplateWithId_P1 {
  NotificationTemplateWithId() = default;

  NotificationTemplateWithId(schemas::ReturnedNotificationTemplate &&a0,
                             schemas::NotificationTemplateWithId_P1 &&a1)
      : schemas::ReturnedNotificationTemplate(std::move(a0)),
        schemas::NotificationTemplateWithId_P1(std::move(a1)) {};

  NotificationTemplateWithId(const std::string &notification_template_id, const std::string &master_id,
                             const std::string &name, const std::optional<std::string> &message_text)
                             : ReturnedNotificationTemplate(master_id, name, message_text),
                               NotificationTemplateWithId_P1(notification_template_id) {};
};

bool operator==(const schemas::NotificationTemplateWithId_P1 &lhs,
                const schemas::NotificationTemplateWithId_P1 &rhs);

bool operator==(const schemas::NotificationTemplateWithId &lhs,
                const schemas::NotificationTemplateWithId &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithId_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithId &value);

NotificationTemplateWithId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateWithId_P1>);

NotificationTemplateWithId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::NotificationTemplateWithId>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateWithId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateWithId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

using NotificationTemplateWithIdList =
    std::vector<schemas::NotificationTemplateWithId>;

// NotificationTemplate object without notificationTemplateId. Used to represent
// user json payloads
struct NotificationTemplateWithoutId_P1 {
  USERVER_NAMESPACE::formats::json::Value extra;
};

struct NotificationTemplateWithoutId
    : public schemas::BaseNotificationTemplate,
      public schemas::NotificationTemplateWithoutId_P1 {
  NotificationTemplateWithoutId() = default;

  NotificationTemplateWithoutId(schemas::BaseNotificationTemplate &&a0,
                                schemas::NotificationTemplateWithoutId_P1 &&a1)
      : schemas::BaseNotificationTemplate(std::move(a0)),
        schemas::NotificationTemplateWithoutId_P1(std::move(a1)) {}
};

bool operator==(const schemas::NotificationTemplateWithoutId_P1 &lhs,
                const schemas::NotificationTemplateWithoutId_P1 &rhs);

bool operator==(const schemas::NotificationTemplateWithoutId &lhs,
                const schemas::NotificationTemplateWithoutId &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithoutId_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithoutId &value);

NotificationTemplateWithoutId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateWithoutId_P1>);

NotificationTemplateWithoutId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateWithoutId>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateWithoutId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateWithoutId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// Base schema for all Recipient schemas returned by the server
struct ReturnedRecipient_P1 {
  std::string master_id{};

  USERVER_NAMESPACE::formats::json::Value extra;
  ReturnedRecipient_P1() = default;
  ReturnedRecipient_P1(const std::string &master_id) : master_id(master_id) {}
};

struct ReturnedRecipient : public schemas::BaseRecipient,
                           public schemas::ReturnedRecipient_P1 {
  ReturnedRecipient() = default;

  ReturnedRecipient(schemas::BaseRecipient &&a0,
                    schemas::ReturnedRecipient_P1 &&a1)
      : schemas::BaseRecipient(std::move(a0)),
        schemas::ReturnedRecipient_P1(std::move(a1)) {};

  ReturnedRecipient(const std::string &master_id, const std::string &name, const std::optional<std::string> &email,
                    const std::optional<std::string> &phone_number, const std::optional<std::string> &telegram_username)
      : BaseRecipient(name, email, phone_number, telegram_username),
        ReturnedRecipient_P1(master_id) {};
};

bool operator==(const schemas::ReturnedRecipient_P1 &lhs,
                const schemas::ReturnedRecipient_P1 &rhs);

bool operator==(const schemas::ReturnedRecipient &lhs,
                const schemas::ReturnedRecipient &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipient_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipient &value);

ReturnedRecipient_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipient_P1>);

ReturnedRecipient Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipient>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedRecipient_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedRecipient &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// Recipient draft object. Used to represent a Recipient object pending for
// creation
struct RecipientDraft_P1 {
  std::string draft_id{};

  USERVER_NAMESPACE::formats::json::Value extra;
  RecipientDraft_P1(const std::string &draft_id) : draft_id(draft_id) {}
  RecipientDraft_P1() = default;
};

struct RecipientDraft : public schemas::ReturnedRecipient,
                        public schemas::RecipientDraft_P1 {
  RecipientDraft() = default;

  RecipientDraft(schemas::ReturnedRecipient &&a0,
                 schemas::RecipientDraft_P1 &&a1)
      : schemas::ReturnedRecipient(std::move(a0)),
        schemas::RecipientDraft_P1(std::move(a1)) {};

  RecipientDraft(const std::string &draft_id, const std::string &master_id, const std::string &name,
                 const std::optional<std::string> &email, const std::optional<std::string> &phone_number,
                 const std::optional<std::string> &telegram_username)
      : ReturnedRecipient(master_id, name, email, phone_number, telegram_username),
        RecipientDraft_P1(draft_id) {};
};

bool operator==(const schemas::RecipientDraft_P1 &lhs,
                const schemas::RecipientDraft_P1 &rhs);

bool operator==(const schemas::RecipientDraft &lhs,
                const schemas::RecipientDraft &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientDraft_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientDraft &value);

RecipientDraft_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientDraft_P1>);

RecipientDraft Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientDraft>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientDraft_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientDraft &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// Base schema for all RecipientGroup schemas returned by the server
struct ReturnedRecipientGroup_P1 {
  std::string master_id{};

  USERVER_NAMESPACE::formats::json::Value extra;
};

struct ReturnedRecipientGroup : public schemas::BaseRecipientGroup,
                                public schemas::ReturnedRecipientGroup_P1 {
  ReturnedRecipientGroup() = default;

  ReturnedRecipientGroup(schemas::BaseRecipientGroup &&a0,
                         schemas::ReturnedRecipientGroup_P1 &&a1)
      : schemas::BaseRecipientGroup(std::move(a0)),
        schemas::ReturnedRecipientGroup_P1(std::move(a1)) {}
};

bool operator==(const schemas::ReturnedRecipientGroup_P1 &lhs,
                const schemas::ReturnedRecipientGroup_P1 &rhs);

bool operator==(const schemas::ReturnedRecipientGroup &lhs,
                const schemas::ReturnedRecipientGroup &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipientGroup_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipientGroup &value);

ReturnedRecipientGroup_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipientGroup_P1>);

ReturnedRecipientGroup Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipientGroup>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedRecipientGroup_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedRecipientGroup &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// Recipient draft object. Used to represent a RecipientGroup object pending for
// creation
struct RecipientGroupDraft_P1 {
  std::string draft_id{};

  USERVER_NAMESPACE::formats::json::Value extra;
};

struct RecipientGroupDraft : public schemas::ReturnedRecipientGroup,
                             public schemas::RecipientGroupDraft_P1 {
  RecipientGroupDraft() = default;

  RecipientGroupDraft(schemas::ReturnedRecipientGroup &&a0,
                      schemas::RecipientGroupDraft_P1 &&a1)
      : schemas::ReturnedRecipientGroup(std::move(a0)),
        schemas::RecipientGroupDraft_P1(std::move(a1)) {}
};

bool operator==(const schemas::RecipientGroupDraft_P1 &lhs,
                const schemas::RecipientGroupDraft_P1 &rhs);

bool operator==(const schemas::RecipientGroupDraft &lhs,
                const schemas::RecipientGroupDraft &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupDraft_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupDraft &value);

RecipientGroupDraft_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupDraft_P1>);

RecipientGroupDraft Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupDraft>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupDraft_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupDraft &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// RecipientGroup object with recipientGroupId. Used to represent
// server-generated objects
struct RecipientGroupWithId_P1 {
  std::string recipient_group_id{};

  USERVER_NAMESPACE::formats::json::Value extra;
};

struct RecipientGroupWithId : public schemas::ReturnedRecipientGroup,
                              public schemas::RecipientGroupWithId_P1 {
  RecipientGroupWithId() = default;

  RecipientGroupWithId(schemas::ReturnedRecipientGroup &&a0,
                       schemas::RecipientGroupWithId_P1 &&a1)
      : schemas::ReturnedRecipientGroup(std::move(a0)),
        schemas::RecipientGroupWithId_P1(std::move(a1)) {}
};

bool operator==(const schemas::RecipientGroupWithId_P1 &lhs,
                const schemas::RecipientGroupWithId_P1 &rhs);

bool operator==(const schemas::RecipientGroupWithId &lhs,
                const schemas::RecipientGroupWithId &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithId_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithId &value);

RecipientGroupWithId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithId_P1>);

RecipientGroupWithId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithId>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupWithId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupWithId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

using RecipientGroupWithIdList = std::vector<schemas::RecipientGroupWithId>;

// RecipientGroup object without recipientGroupId. Used to represent user json
// payloads
struct RecipientGroupWithoutId_P1 {
  USERVER_NAMESPACE::formats::json::Value extra;
};

struct RecipientGroupWithoutId : public schemas::BaseRecipientGroup,
                                 public schemas::RecipientGroupWithoutId_P1 {
  RecipientGroupWithoutId() = default;

  RecipientGroupWithoutId(schemas::BaseRecipientGroup &&a0,
                          schemas::RecipientGroupWithoutId_P1 &&a1)
      : schemas::BaseRecipientGroup(std::move(a0)),
        schemas::RecipientGroupWithoutId_P1(std::move(a1)) {}
};

bool operator==(const schemas::RecipientGroupWithoutId_P1 &lhs,
                const schemas::RecipientGroupWithoutId_P1 &rhs);

bool operator==(const schemas::RecipientGroupWithoutId &lhs,
                const schemas::RecipientGroupWithoutId &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithoutId_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithoutId &value);

RecipientGroupWithoutId_P1 Parse(USERVER_NAMESPACE::formats::json::Value json,
                                 USERVER_NAMESPACE::formats::parse::To<
                                     schemas::RecipientGroupWithoutId_P1>);

RecipientGroupWithoutId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithoutId>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupWithoutId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupWithoutId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// Recipient object with recipient_id. Used to represent server-generated
// objects
struct RecipientWithId_P1 {
  std::string recipient_id{};

  USERVER_NAMESPACE::formats::json::Value extra;
  RecipientWithId_P1(const std::string &recipient_id) : recipient_id(recipient_id) {};
  RecipientWithId_P1() = default;
};

struct RecipientWithId : public schemas::ReturnedRecipient,
                         public schemas::RecipientWithId_P1 {
  RecipientWithId() = default;

  RecipientWithId(schemas::ReturnedRecipient &&a0,
                  schemas::RecipientWithId_P1 &&a1)
      : schemas::ReturnedRecipient(std::move(a0)),
        schemas::RecipientWithId_P1(std::move(a1)) {};

  RecipientWithId(const std::string &recipient_id, const std::string &master_id, const std::string &name,
                  const std::optional<std::string> &email, const std::optional<std::string> &phone_number,
                  const std::optional<std::string> &telegram_username)
      : ReturnedRecipient(master_id, name, email, phone_number, telegram_username),
        RecipientWithId_P1(recipient_id) {};
};

bool operator==(const schemas::RecipientWithId_P1 &lhs,
                const schemas::RecipientWithId_P1 &rhs);

bool operator==(const schemas::RecipientWithId &lhs,
                const schemas::RecipientWithId &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithId_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithId &value);

RecipientWithId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithId_P1>);

RecipientWithId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithId>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientWithId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientWithId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

using RecipientWithIdList = std::vector<schemas::RecipientWithId>;

// Recipient object without recipient_id. Used to represent user json payloads
struct RecipientWithoutId_P1 {
  USERVER_NAMESPACE::formats::json::Value extra;
};

struct RecipientWithoutId : public schemas::BaseRecipient,
                            public schemas::RecipientWithoutId_P1 {
  RecipientWithoutId() = default;

  RecipientWithoutId(schemas::BaseRecipient &&a0,
                     schemas::RecipientWithoutId_P1 &&a1)
      : schemas::BaseRecipient(std::move(a0)),
        schemas::RecipientWithoutId_P1(std::move(a1)) {}
};

bool operator==(const schemas::RecipientWithoutId_P1 &lhs,
                const schemas::RecipientWithoutId_P1 &rhs);

bool operator==(const schemas::RecipientWithoutId &lhs,
                const schemas::RecipientWithoutId &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithoutId_P1 &value);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithoutId &value);

RecipientWithoutId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithoutId_P1>);

RecipientWithoutId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithoutId>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientWithoutId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientWithoutId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

// User Login schema
struct User {
  std::string name{};
  std::string password{};
};

bool operator==(const schemas::User &lhs, const schemas::User &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh, const schemas::User &value);

User Parse(USERVER_NAMESPACE::formats::json::Value json,
           USERVER_NAMESPACE::formats::parse::To<schemas::User>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::User &value, USERVER_NAMESPACE::formats::serialize::To<
    USERVER_NAMESPACE::formats::json::Value>);

}  // namespace schemas
