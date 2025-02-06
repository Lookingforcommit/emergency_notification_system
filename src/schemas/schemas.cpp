#include "schemas.hpp"

#include <userver/chaotic/type_bundle_cpp.hpp>
#include <utility>
#include <userver/formats/common/merge.hpp>

#include "schemas_parsers.ipp"

namespace schemas {

bool operator==(const schemas::BaseNotificationTemplate &lhs,
                const schemas::BaseNotificationTemplate &rhs) {
  return lhs.name == rhs.name && lhs.message_text == rhs.message_text;
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::BaseNotificationTemplate &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

BaseNotificationTemplate Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseNotificationTemplate>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::BaseNotificationTemplate &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;

  vb["name"] = USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.name};

  if (value.message_text) {
    vb["message_text"] =
        USERVER_NAMESPACE::chaotic::Primitive<std::string>{*value.message_text};
  }

  return vb.ExtractValue();
}

bool operator==(const schemas::BaseRecipient &lhs,
                const schemas::BaseRecipient &rhs) {
  return lhs.name == rhs.name && lhs.email == rhs.email &&
      lhs.phone_number == rhs.phone_number &&
      lhs.telegram_id == rhs.telegram_id;
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::BaseRecipient &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

BaseRecipient Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseRecipient> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::BaseRecipient &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;

  vb["name"] = USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.name};

  if (value.email) {
    vb["email"] =
        USERVER_NAMESPACE::chaotic::Primitive<std::string>{*value.email};
  }

  if (value.phone_number) {
    vb["phone_number"] =
        USERVER_NAMESPACE::chaotic::Primitive<std::string>{*value.phone_number};
  }

  if (value.telegram_id) {
    vb["telegram_id"] =
        USERVER_NAMESPACE::chaotic::Primitive<int64_t>{
            *value.telegram_id};
  }

  return vb.ExtractValue();
}

bool operator==(const schemas::BaseRecipientGroup &lhs,
                const schemas::BaseRecipientGroup &rhs) {
  return lhs.name == rhs.name &&
      lhs.notification_template_id == rhs.notification_template_id &&
      lhs.active == rhs.active;
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::BaseRecipientGroup &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

BaseRecipientGroup Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseRecipientGroup> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::BaseRecipientGroup &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;

  vb["name"] = USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.name};

  if (value.notification_template_id) {
    vb["notification_template_id"] =
        USERVER_NAMESPACE::chaotic::Primitive<std::string>{
            *value.notification_template_id};
  }

  vb["active"] = USERVER_NAMESPACE::chaotic::Primitive<bool>{value.active};

  return vb.ExtractValue();
}

bool operator==(const schemas::Notification &lhs,
                const schemas::Notification &rhs) {
  return lhs.batch_id == rhs.batch_id &&
      lhs.recipient_id == rhs.recipient_id && lhs.group_id == rhs.group_id &&
      lhs.type == rhs.type &&
      lhs.creation_timestamp == rhs.creation_timestamp &&
      lhs.completion_timestamp == rhs.completion_timestamp;
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::Notification::Type &value) {
  return lh << ToString(value);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::Notification &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

Notification::Type Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification::Type> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

Notification Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

schemas::Notification::Type FromString(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification::Type>) {
  const auto result =
      kschemas_Notification_Type_Mapping.TryFindBySecond(value);
  if (result.has_value()) {
    return *result;
  }
  throw std::runtime_error(fmt::format(
      "Invalid enum value ({}) for type schemas::Notification::Type", value));
}

schemas::Notification::Type Parse(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification::Type> to) {
  return FromString(value, to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::Notification::Type &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  return USERVER_NAMESPACE::formats::json::ValueBuilder(ToString(value))
      .ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::Notification &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;

  vb["batch_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.batch_id};

  vb["recipient_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.recipient_id};

  vb["group_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.group_id};

  vb["type"] =
      USERVER_NAMESPACE::chaotic::Primitive<schemas::Notification::Type>{
          value.type};

  vb["creation_timestamp"] = USERVER_NAMESPACE::chaotic::Primitive<std::string>{
      value.creation_timestamp};

  vb["completion_timestamp"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::optional<std::string>>{
          value.completion_timestamp};

  return vb.ExtractValue();
}

std::string ToString(schemas::Notification::Type value) {
  const auto result =
      kschemas_Notification_Type_Mapping.TryFindByFirst(value);
  if (result.has_value()) {
    return std::string{*result};
  }
  throw std::runtime_error("Bad enum value");
}

bool operator==(const schemas::NotificationsBatch &lhs,
                const schemas::NotificationsBatch &rhs) {
  return lhs.batch_id == rhs.batch_id &&
      lhs.master_id == rhs.master_id;
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationsBatch &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

NotificationsBatch Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::NotificationsBatch> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::NotificationsBatch &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;

  vb["master_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.master_id};

  return vb.ExtractValue();
}

bool operator==(const schemas::ReturnedNotificationTemplate_P1 &lhs,
                const schemas::ReturnedNotificationTemplate_P1 &rhs) {
  return lhs.master_id == rhs.master_id && lhs.extra == rhs.extra;
}

bool operator==(const schemas::ReturnedNotificationTemplate &lhs,
                const schemas::ReturnedNotificationTemplate &rhs) {
  return static_cast<const schemas::BaseNotificationTemplate &>(lhs) ==
      static_cast<const schemas::BaseNotificationTemplate &>(rhs) &&
      static_cast<const schemas::ReturnedNotificationTemplate_P1 &>(lhs) ==
          static_cast<const schemas::ReturnedNotificationTemplate_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedNotificationTemplate_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedNotificationTemplate &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

ReturnedNotificationTemplate_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::ReturnedNotificationTemplate_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

ReturnedNotificationTemplate Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedNotificationTemplate>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::ReturnedNotificationTemplate_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["master_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.master_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedNotificationTemplate &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::BaseNotificationTemplate>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedNotificationTemplate_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::NotificationTemplateDraft_P1 &lhs,
                const schemas::NotificationTemplateDraft_P1 &rhs) {
  return lhs.draft_id == rhs.draft_id && lhs.extra == rhs.extra;
}

bool operator==(const schemas::NotificationTemplateDraft &lhs,
                const schemas::NotificationTemplateDraft &rhs) {
  return static_cast<const schemas::ReturnedNotificationTemplate &>(lhs) ==
      static_cast<const schemas::ReturnedNotificationTemplate &>(rhs) &&
      static_cast<const schemas::NotificationTemplateDraft_P1 &>(lhs) ==
          static_cast<const schemas::NotificationTemplateDraft_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateDraft_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateDraft &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

NotificationTemplateDraft_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateDraft_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

NotificationTemplateDraft Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::NotificationTemplateDraft>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::NotificationTemplateDraft_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["draft_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.draft_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateDraft &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedNotificationTemplate>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::NotificationTemplateDraft_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::NotificationTemplateWithId_P1 &lhs,
                const schemas::NotificationTemplateWithId_P1 &rhs) {
  return lhs.notification_template_id == rhs.notification_template_id &&
      lhs.extra == rhs.extra;
}

bool operator==(const schemas::NotificationTemplateWithId &lhs,
                const schemas::NotificationTemplateWithId &rhs) {
  return static_cast<const schemas::ReturnedNotificationTemplate &>(lhs) ==
      static_cast<const schemas::ReturnedNotificationTemplate &>(rhs) &&
      static_cast<const schemas::NotificationTemplateWithId_P1 &>(lhs) ==
          static_cast<const schemas::NotificationTemplateWithId_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithId_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithId &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

NotificationTemplateWithId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateWithId_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

NotificationTemplateWithId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::NotificationTemplateWithId>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::NotificationTemplateWithId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["notification_template_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{
          value.notification_template_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateWithId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedNotificationTemplate>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::NotificationTemplateWithId_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::NotificationTemplateWithoutId_P1 &lhs,
                const schemas::NotificationTemplateWithoutId_P1 &rhs) {
  (void) lhs;
  (void) rhs;

  return

      lhs.extra == rhs.extra;
}

bool operator==(const schemas::NotificationTemplateWithoutId &lhs,
                const schemas::NotificationTemplateWithoutId &rhs) {
  return static_cast<const schemas::BaseNotificationTemplate &>(lhs) ==
      static_cast<const schemas::BaseNotificationTemplate &>(rhs) &&
      static_cast<const schemas::NotificationTemplateWithoutId_P1 &>(lhs) ==
          static_cast<const schemas::NotificationTemplateWithoutId_P1 &>(
              rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithoutId_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::NotificationTemplateWithoutId &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

NotificationTemplateWithoutId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateWithoutId_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

NotificationTemplateWithoutId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<
        schemas::NotificationTemplateWithoutId>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::NotificationTemplateWithoutId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::NotificationTemplateWithoutId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::BaseNotificationTemplate>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::NotificationTemplateWithoutId_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::ReturnedRecipient_P1 &lhs,
                const schemas::ReturnedRecipient_P1 &rhs) {
  return lhs.master_id == rhs.master_id && lhs.extra == rhs.extra;
}

bool operator==(const schemas::ReturnedRecipient &lhs,
                const schemas::ReturnedRecipient &rhs) {
  return static_cast<const schemas::BaseRecipient &>(lhs) ==
      static_cast<const schemas::BaseRecipient &>(rhs) &&
      static_cast<const schemas::ReturnedRecipient_P1 &>(lhs) ==
          static_cast<const schemas::ReturnedRecipient_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipient_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipient &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

ReturnedRecipient_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipient_P1> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

ReturnedRecipient Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipient> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::ReturnedRecipient_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["master_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.master_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedRecipient &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::BaseRecipient>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedRecipient_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::RecipientDraft_P1 &lhs,
                const schemas::RecipientDraft_P1 &rhs) {
  return lhs.draft_id == rhs.draft_id && lhs.extra == rhs.extra;
}

bool operator==(const schemas::RecipientDraft &lhs,
                const schemas::RecipientDraft &rhs) {
  return static_cast<const schemas::ReturnedRecipient &>(lhs) ==
      static_cast<const schemas::ReturnedRecipient &>(rhs) &&
      static_cast<const schemas::RecipientDraft_P1 &>(lhs) ==
          static_cast<const schemas::RecipientDraft_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientDraft_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientDraft &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

RecipientDraft_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientDraft_P1> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

RecipientDraft Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientDraft> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::RecipientDraft_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["draft_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.draft_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientDraft &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedRecipient>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::RecipientDraft_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::ReturnedRecipientGroup_P1 &lhs,
                const schemas::ReturnedRecipientGroup_P1 &rhs) {
  return lhs.master_id == rhs.master_id && lhs.extra == rhs.extra;
}

bool operator==(const schemas::ReturnedRecipientGroup &lhs,
                const schemas::ReturnedRecipientGroup &rhs) {
  return static_cast<const schemas::BaseRecipientGroup &>(lhs) ==
      static_cast<const schemas::BaseRecipientGroup &>(rhs) &&
      static_cast<const schemas::ReturnedRecipientGroup_P1 &>(lhs) ==
          static_cast<const schemas::ReturnedRecipientGroup_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipientGroup_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::ReturnedRecipientGroup &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

ReturnedRecipientGroup_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipientGroup_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

ReturnedRecipientGroup Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipientGroup> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::ReturnedRecipientGroup_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["master_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.master_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::ReturnedRecipientGroup &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::BaseRecipientGroup>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedRecipientGroup_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::RecipientGroupDraft_P1 &lhs,
                const schemas::RecipientGroupDraft_P1 &rhs) {
  return lhs.draft_id == rhs.draft_id && lhs.extra == rhs.extra;
}

bool operator==(const schemas::RecipientGroupDraft &lhs,
                const schemas::RecipientGroupDraft &rhs) {
  return static_cast<const schemas::ReturnedRecipientGroup &>(lhs) ==
      static_cast<const schemas::ReturnedRecipientGroup &>(rhs) &&
      static_cast<const schemas::RecipientGroupDraft_P1 &>(lhs) ==
          static_cast<const schemas::RecipientGroupDraft_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupDraft_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupDraft &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

RecipientGroupDraft_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupDraft_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

RecipientGroupDraft Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupDraft> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::RecipientGroupDraft_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["draft_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.draft_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupDraft &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedRecipientGroup>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::RecipientGroupDraft_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::RecipientGroupWithId_P1 &lhs,
                const schemas::RecipientGroupWithId_P1 &rhs) {
  return lhs.recipient_group_id == rhs.recipient_group_id &&
      lhs.extra == rhs.extra;
}

bool operator==(const schemas::RecipientGroupWithId &lhs,
                const schemas::RecipientGroupWithId &rhs) {
  return static_cast<const schemas::ReturnedRecipientGroup &>(lhs) ==
      static_cast<const schemas::ReturnedRecipientGroup &>(rhs) &&
      static_cast<const schemas::RecipientGroupWithId_P1 &>(lhs) ==
          static_cast<const schemas::RecipientGroupWithId_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithId_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithId &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

RecipientGroupWithId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithId_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

RecipientGroupWithId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithId> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::RecipientGroupWithId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["recipient_group_id"] = USERVER_NAMESPACE::chaotic::Primitive<std::string>{
      value.recipient_group_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupWithId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedRecipientGroup>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::RecipientGroupWithId_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::RecipientGroupWithoutId_P1 &lhs,
                const schemas::RecipientGroupWithoutId_P1 &rhs) {
  (void) lhs;
  (void) rhs;

  return

      lhs.extra == rhs.extra;
}

bool operator==(const schemas::RecipientGroupWithoutId &lhs,
                const schemas::RecipientGroupWithoutId &rhs) {
  return static_cast<const schemas::BaseRecipientGroup &>(lhs) ==
      static_cast<const schemas::BaseRecipientGroup &>(rhs) &&
      static_cast<const schemas::RecipientGroupWithoutId_P1 &>(lhs) ==
          static_cast<const schemas::RecipientGroupWithoutId_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithoutId_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientGroupWithoutId &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

RecipientGroupWithoutId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithoutId_P1>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

RecipientGroupWithoutId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithoutId>
    to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::RecipientGroupWithoutId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientGroupWithoutId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::BaseRecipientGroup>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::RecipientGroupWithoutId_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::RecipientWithId_P1 &lhs,
                const schemas::RecipientWithId_P1 &rhs) {
  return lhs.recipient_id == rhs.recipient_id && lhs.extra == rhs.extra;
}

bool operator==(const schemas::RecipientWithId &lhs,
                const schemas::RecipientWithId &rhs) {
  return static_cast<const schemas::ReturnedRecipient &>(lhs) ==
      static_cast<const schemas::ReturnedRecipient &>(rhs) &&
      static_cast<const schemas::RecipientWithId_P1 &>(lhs) ==
          static_cast<const schemas::RecipientWithId_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithId_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithId &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

RecipientWithId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithId_P1> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

RecipientWithId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithId> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::RecipientWithId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  vb["recipient_id"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.recipient_id};

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientWithId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::ReturnedRecipient>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::RecipientWithId_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::RecipientWithoutId_P1 &lhs,
                const schemas::RecipientWithoutId_P1 &rhs) {
  (void) lhs;
  (void) rhs;

  return

      lhs.extra == rhs.extra;
}

bool operator==(const schemas::RecipientWithoutId &lhs,
                const schemas::RecipientWithoutId &rhs) {
  return static_cast<const schemas::BaseRecipient &>(lhs) ==
      static_cast<const schemas::BaseRecipient &>(rhs) &&
      static_cast<const schemas::RecipientWithoutId_P1 &>(lhs) ==
          static_cast<const schemas::RecipientWithoutId_P1 &>(rhs);
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithoutId_P1 &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh,
    const schemas::RecipientWithoutId &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

RecipientWithoutId_P1 Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithoutId_P1> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

RecipientWithoutId Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithoutId> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::RecipientWithoutId_P1 &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb = value.extra;

  return vb.ExtractValue();
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    const schemas::RecipientWithoutId &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::BaseRecipient>(value)}
          .ExtractValue());
  USERVER_NAMESPACE::formats::common::Merge(
      vb,
      USERVER_NAMESPACE::formats::json::ValueBuilder{
          static_cast<schemas::RecipientWithoutId_P1>(value)}
          .ExtractValue());
  return vb.ExtractValue();
}

bool operator==(const schemas::User &lhs, const schemas::User &rhs) {
  return lhs.name == rhs.name && lhs.password == rhs.password;
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh, const schemas::User &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

User Parse(USERVER_NAMESPACE::formats::json::Value json,
           USERVER_NAMESPACE::formats::parse::To<schemas::User> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::User &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;

  vb["name"] = USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.name};

  vb["password"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.password};

  return vb.ExtractValue();
}

bool operator==(const schemas::JWTPair &lhs, const schemas::JWTPair &rhs) {
  return lhs.access_token == rhs.access_token && lhs.refresh_token == rhs.refresh_token;
}

USERVER_NAMESPACE::logging::LogHelper &operator<<(
    USERVER_NAMESPACE::logging::LogHelper &lh, const schemas::JWTPair &value) {
  return lh << ToString(USERVER_NAMESPACE::formats::json::ValueBuilder(value)
                            .ExtractValue());
}

JWTPair Parse(USERVER_NAMESPACE::formats::json::Value json,
              USERVER_NAMESPACE::formats::parse::To<schemas::JWTPair> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(std::move(json), to);
}

USERVER_NAMESPACE::formats::json::Value Serialize(
    [[maybe_unused]] const schemas::JWTPair &value,
    USERVER_NAMESPACE::formats::serialize::To<
        USERVER_NAMESPACE::formats::json::Value>) {
  USERVER_NAMESPACE::formats::json::ValueBuilder vb =
      USERVER_NAMESPACE::formats::common::Type::kObject;

  vb["access_token"] = USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.access_token};

  vb["refresh_token"] =
      USERVER_NAMESPACE::chaotic::Primitive<std::string>{value.refresh_token};

  return vb.ExtractValue();
}
}  // namespace schemas
