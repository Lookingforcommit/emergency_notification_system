#pragma once

#include <optional>
#include <string>

#include <userver/formats/json/value.hpp>
#include <userver/chaotic/type_bundle_hpp.hpp>
#include <userver/utils/trivial_map.hpp>
#include <userver/storages/postgres/io/type_mapping.hpp>
#include <userver/storages/postgres/io/user_types.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include <userver/storages/postgres/io/enum_types.hpp>

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
  std::optional<int64_t> telegram_id{};
  BaseRecipient(const std::string &name, const std::optional<std::string> &email,
                const std::optional<std::string> &phone_number, const std::optional<int64_t> &telegram_id)
      : name(name),
        email(email),
        phone_number(phone_number),
        telegram_id(telegram_id) {}
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

  BaseRecipientGroup() = default;

  BaseRecipientGroup(const std::string &name, const std::optional<std::string> &notification_template_id, bool active)
      : name(name),
        notification_template_id(notification_template_id),
        active(active) {};
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

  std::string notification_id{};
  std::string batch_id{};
  std::string recipient_id{};
  std::string group_id{};
  schemas::Notification::Type type{};
  std::string creation_timestamp{};
  std::optional<std::string> completion_timestamp{};
};

static constexpr USERVER_NAMESPACE::utils::TrivialBiMap
    kschemas_Notification_Type_Mapping = [](auto selector) {
  return selector()
      .template Type<schemas::Notification::Type, std::string_view>()
      .Case(schemas::Notification::Type::kTelegram, "Telegram")
      .Case(schemas::Notification::Type::kSms, "SMS")
      .Case(schemas::Notification::Type::kMail, "Mail");
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

struct NotificationsBatch {
  std::string batch_id{};
  std::string master_id{};
};

bool operator==(const schemas::NotificationsBatch &lhs,
                const schemas::NotificationsBatch &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationsBatch &value);

NotificationsBatch Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::NotificationsBatch>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationsBatch &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>);

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
                    const std::optional<std::string> &phone_number, const std::optional<int64_t> &telegram_id)
      : BaseRecipient(name, email, phone_number, telegram_id),
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
                 const std::optional<int64_t> &telegram_id)
      : ReturnedRecipient(master_id, name, email, phone_number, telegram_id),
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

  ReturnedRecipientGroup_P1() = default;

  ReturnedRecipientGroup_P1(const std::string &master_id) : master_id(master_id) {}
};

struct ReturnedRecipientGroup : public schemas::BaseRecipientGroup,
                                public schemas::ReturnedRecipientGroup_P1 {
  ReturnedRecipientGroup() = default;

  ReturnedRecipientGroup(schemas::BaseRecipientGroup &&a0,
                         schemas::ReturnedRecipientGroup_P1 &&a1)
      : schemas::BaseRecipientGroup(std::move(a0)),
        schemas::ReturnedRecipientGroup_P1(std::move(a1)) {};

  ReturnedRecipientGroup(const std::string &master_id, const std::string &name,
                         const std::optional<std::string> &notification_template_id, bool active)
      : BaseRecipientGroup(name, notification_template_id, active),
        ReturnedRecipientGroup_P1(master_id) {};
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

  RecipientGroupDraft_P1() = default;

  RecipientGroupDraft_P1(const std::string &draft_id) : draft_id(draft_id) {};
};

struct RecipientGroupDraft : public schemas::ReturnedRecipientGroup,
                             public schemas::RecipientGroupDraft_P1 {
  RecipientGroupDraft() = default;

  RecipientGroupDraft(schemas::ReturnedRecipientGroup &&a0,
                      schemas::RecipientGroupDraft_P1 &&a1)
      : schemas::ReturnedRecipientGroup(std::move(a0)),
        schemas::RecipientGroupDraft_P1(std::move(a1)) {};

  RecipientGroupDraft(const std::string &draft_id, const std::string &master_id, const std::string &name,
                      const std::optional<std::string> &notification_template_id, bool active)
      : ReturnedRecipientGroup(master_id, name, notification_template_id, active),
        RecipientGroupDraft_P1(draft_id) {};
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

  RecipientGroupWithId_P1() = default;

  RecipientGroupWithId_P1(const std::string &recipient_group_id) : recipient_group_id(recipient_group_id) {};
};

struct RecipientGroupWithId : public schemas::ReturnedRecipientGroup,
                              public schemas::RecipientGroupWithId_P1 {
  RecipientGroupWithId() = default;

  RecipientGroupWithId(schemas::ReturnedRecipientGroup &&a0,
                       schemas::RecipientGroupWithId_P1 &&a1)
      : schemas::ReturnedRecipientGroup(std::move(a0)),
        schemas::RecipientGroupWithId_P1(std::move(a1)) {};

  RecipientGroupWithId(const std::string &recipient_group_id, const std::string &master_id, const std::string &name,
                       const std::optional<std::string> &notification_template_id, bool active)
      : ReturnedRecipientGroup(master_id, name, notification_template_id, active),
        RecipientGroupWithId_P1(recipient_group_id) {};
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
                  const std::optional<int64_t> &telegram_id)
      : ReturnedRecipient(master_id, name, email, phone_number, telegram_id),
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

// Returned JWT tokens schema
struct JWTPair {
  std::string access_token{};
  std::string refresh_token{};

  JWTPair() = default;

  JWTPair(const std::string &access_token,
          const std::string &refresh_token)
      : access_token(access_token),
        refresh_token(refresh_token) {}
};

bool operator==(const schemas::JWTPair &lhs, const schemas::JWTPair &rhs);

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh, const schemas::JWTPair &value);

JWTPair Parse(USERVER_NAMESPACE::formats::json::Value json,
              USERVER_NAMESPACE::formats::parse::To<schemas::JWTPair>);

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::JWTPair &value, USERVER_NAMESPACE::formats::serialize::To<
    USERVER_NAMESPACE::formats::json::Value>);

}  // namespace schemas

// Postgres to cpp type mappings
namespace USERVER_NAMESPACE::storages::postgres::io {
template<>
struct CppToUserPg<schemas::Notification::Type> {
  static constexpr userver::storages::postgres::DBTypeName postgres_name = "ens_schema.message_type";
  static constexpr userver::utils::TrivialBiMap enumerators = schemas::kschemas_Notification_Type_Mapping;
};
}
