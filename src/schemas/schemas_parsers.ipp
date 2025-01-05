#pragma once

#include "schemas.hpp"

#include <userver/chaotic/array.hpp>
#include <userver/chaotic/exception.hpp>
#include <userver/chaotic/object.hpp>
#include <userver/chaotic/primitive.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/utils/trivial_map.hpp>

namespace schemas {

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_BaseNotificationTemplate_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("name").Case(
      "message_text");
};

template<typename Value>
schemas::BaseNotificationTemplate Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseNotificationTemplate>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::BaseNotificationTemplate res;

  res.name =
      value["name"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.message_text =
      value["message_text"]
          .template As<std::optional<
              USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, kschemas_BaseNotificationTemplate_PropertiesNames);

  return res;
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_BaseRecipient_PropertiesNames = [](auto selector) {
  return selector()
      .template Type<std::string_view>()
      .Case("name")
      .Case("email")
      .Case("phone_number")
      .Case("telegram_username");
};

template<typename Value>
schemas::BaseRecipient Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseRecipient>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::BaseRecipient res;

  res.name =
      value["name"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.email = value["email"]
      .template As<std::optional<
          USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();
  res.phone_number =
      value["phone_number"]
          .template As<std::optional<
              USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();
  res.telegram_username =
      value["telegram_username"]
          .template As<std::optional<
              USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, kschemas_BaseRecipient_PropertiesNames);

  return res;
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_BaseRecipientGroup_PropertiesNames = [](auto selector) {
  return selector()
      .template Type<std::string_view>()
      .Case("name")
      .Case("notification_template_id")
      .Case("active");
};

template<typename Value>
schemas::BaseRecipientGroup Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::BaseRecipientGroup>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::BaseRecipientGroup res;

  res.name =
      value["name"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.notification_template_id =
      value["notification_template_id"]
          .template As<std::optional<
              USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();
  res.active = value["active"]
      .template As<USERVER_NAMESPACE::chaotic::Primitive<bool>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, kschemas_BaseRecipientGroup_PropertiesNames);

  return res;
}

static constexpr USERVER_NAMESPACE::utils::TrivialBiMap
    kschemas_Notification_Type_Mapping = [](auto selector) {
  return selector()
      .template Type<schemas::Notification::Type, std::string_view>()
      .Case(schemas::Notification::Type::kTelegram, "Telegram")
      .Case(schemas::Notification::Type::kSms, "SMS")
      .Case(schemas::Notification::Type::kMail, "Mail");
};

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_Notification_PropertiesNames = [](auto selector) {
  return selector()
      .template Type<std::string_view>()
      .Case("master_id")
      .Case("recipient_id")
      .Case("group_id")
      .Case("type")
      .Case("creation_timestamp")
      .Case("completion_timestamp");
};

template<typename Value>
schemas::Notification::Type Parse(
    Value val,
    USERVER_NAMESPACE::formats::parse::To<schemas::Notification::Type>) {
  const auto value = val.template As<std::string>();
  const auto result = kschemas_Notification_Type_Mapping.TryFindBySecond(value);
  if (result.has_value()) {
    return *result;
  }
  USERVER_NAMESPACE::chaotic::ThrowForValue(
      fmt::format(
          "Invalid enum value ({}) for type schemas::Notification::Type",
          value),
      val);
}

template<typename Value>
schemas::Notification Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<schemas::Notification>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::Notification res;

  res.master_id =
      value["master_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.recipient_id =
      value["recipient_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.group_id =
      value["group_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.type = value["type"]
      .template As<USERVER_NAMESPACE::chaotic::Primitive<
          schemas::Notification::Type>>();
  res.creation_timestamp =
      value["creation_timestamp"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.completion_timestamp =
      value["completion_timestamp"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, kschemas_Notification_PropertiesNames);

  return res;
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_ReturnedNotificationTemplate_P1_PropertiesNames =
    [](auto selector) {
      return selector().template Type<std::string_view>().Case("master_id");
    };

template<typename Value>
schemas::ReturnedNotificationTemplate_P1 Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::ReturnedNotificationTemplate_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::ReturnedNotificationTemplate_P1 res;

  res.master_id =
      value["master_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_ReturnedNotificationTemplate_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::ReturnedNotificationTemplate Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::ReturnedNotificationTemplate>) {
  return schemas::ReturnedNotificationTemplate(
      value.template As<schemas::BaseNotificationTemplate>(),
      value.template As<schemas::ReturnedNotificationTemplate_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_NotificationTemplateDraft_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("draft_id");
};

template<typename Value>
schemas::NotificationTemplateDraft_P1 Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::NotificationTemplateDraft_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::NotificationTemplateDraft_P1 res;

  res.draft_id =
      value["draft_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_NotificationTemplateDraft_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::NotificationTemplateDraft Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::NotificationTemplateDraft>) {
  return schemas::NotificationTemplateDraft(
      value.template As<schemas::ReturnedNotificationTemplate>(),
      value.template As<schemas::NotificationTemplateDraft_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_NotificationTemplateWithId_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case(
      "notification_template_id");
};

template<typename Value>
schemas::NotificationTemplateWithId_P1 Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::NotificationTemplateWithId_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::NotificationTemplateWithId_P1 res;

  res.notification_template_id =
      value["notification_template_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_NotificationTemplateWithId_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::NotificationTemplateWithId Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::NotificationTemplateWithId>) {
  return schemas::NotificationTemplateWithId(
      value.template As<schemas::ReturnedNotificationTemplate>(),
      value.template As<schemas::NotificationTemplateWithId_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_NotificationTemplateWithoutId_P1_PropertiesNames =
    [](auto selector) {
      return selector().template Type<std::string_view>();
    };

template<typename Value>
schemas::NotificationTemplateWithoutId_P1 Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::NotificationTemplateWithoutId_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::NotificationTemplateWithoutId_P1 res;

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_NotificationTemplateWithoutId_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::NotificationTemplateWithoutId Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::NotificationTemplateWithoutId>) {
  return schemas::NotificationTemplateWithoutId(
      value.template As<schemas::BaseNotificationTemplate>(),
      value.template As<schemas::NotificationTemplateWithoutId_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_ReturnedRecipient_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("master_id");
};

template<typename Value>
schemas::ReturnedRecipient_P1 Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipient_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::ReturnedRecipient_P1 res;

  res.master_id =
      value["master_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_ReturnedRecipient_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::ReturnedRecipient Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipient>) {
  return schemas::ReturnedRecipient(
      value.template As<schemas::BaseRecipient>(),
      value.template As<schemas::ReturnedRecipient_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_RecipientDraft_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("draft_id");
};

template<typename Value>
schemas::RecipientDraft_P1 Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientDraft_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::RecipientDraft_P1 res;

  res.draft_id =
      value["draft_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_RecipientDraft_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::RecipientDraft Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientDraft>) {
  return schemas::RecipientDraft(
      value.template As<schemas::ReturnedRecipient>(),
      value.template As<schemas::RecipientDraft_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_ReturnedRecipientGroup_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("master_id");
};

template<typename Value>
schemas::ReturnedRecipientGroup_P1 Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::ReturnedRecipientGroup_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::ReturnedRecipientGroup_P1 res;

  res.master_id =
      value["master_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_ReturnedRecipientGroup_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::ReturnedRecipientGroup Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::ReturnedRecipientGroup>) {
  return schemas::ReturnedRecipientGroup(
      value.template As<schemas::BaseRecipientGroup>(),
      value.template As<schemas::ReturnedRecipientGroup_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_RecipientGroupDraft_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("draft_id");
};

template<typename Value>
schemas::RecipientGroupDraft_P1 Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupDraft_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::RecipientGroupDraft_P1 res;

  res.draft_id =
      value["draft_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_RecipientGroupDraft_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::RecipientGroupDraft Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupDraft>) {
  return schemas::RecipientGroupDraft(
      value.template As<schemas::ReturnedRecipientGroup>(),
      value.template As<schemas::RecipientGroupDraft_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_RecipientGroupWithId_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case(
      "recipient_group_id");
};

template<typename Value>
schemas::RecipientGroupWithId_P1 Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithId_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::RecipientGroupWithId_P1 res;

  res.recipient_group_id =
      value["recipient_group_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_RecipientGroupWithId_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::RecipientGroupWithId Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithId>) {
  return schemas::RecipientGroupWithId(
      value.template As<schemas::ReturnedRecipientGroup>(),
      value.template As<schemas::RecipientGroupWithId_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_RecipientGroupWithoutId_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>();
};

template<typename Value>
schemas::RecipientGroupWithoutId_P1 Parse(
    Value value, USERVER_NAMESPACE::formats::parse::To<
    schemas::RecipientGroupWithoutId_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::RecipientGroupWithoutId_P1 res;

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_RecipientGroupWithoutId_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::RecipientGroupWithoutId Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientGroupWithoutId>) {
  return schemas::RecipientGroupWithoutId(
      value.template As<schemas::BaseRecipientGroup>(),
      value.template As<schemas::RecipientGroupWithoutId_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_RecipientWithId_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("recipient_id");
};

template<typename Value>
schemas::RecipientWithId_P1 Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithId_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::RecipientWithId_P1 res;

  res.recipient_id =
      value["recipient_id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_RecipientWithId_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::RecipientWithId Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithId>) {
  return schemas::RecipientWithId(
      value.template As<schemas::ReturnedRecipient>(),
      value.template As<schemas::RecipientWithId_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_RecipientWithoutId_P1_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>();
};

template<typename Value>
schemas::RecipientWithoutId_P1 Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithoutId_P1>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::RecipientWithoutId_P1 res;

  res.extra = USERVER_NAMESPACE::chaotic::ExtractAdditionalPropertiesTrue(
      value, kschemas_RecipientWithoutId_P1_PropertiesNames);

  return res;
}

template<typename Value>
schemas::RecipientWithoutId Parse(
    Value value,
    USERVER_NAMESPACE::formats::parse::To<schemas::RecipientWithoutId>) {
  return schemas::RecipientWithoutId(
      value.template As<schemas::BaseRecipient>(),
      value.template As<schemas::RecipientWithoutId_P1>());
}

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kschemas_User_PropertiesNames = [](auto selector) {
  return selector().template Type<std::string_view>().Case("name").Case(
      "password");
};

template<typename Value>
schemas::User Parse(Value value,
                    USERVER_NAMESPACE::formats::parse::To<schemas::User>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::User res;

  res.name =
      value["name"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.password =
      value["password"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, kschemas_User_PropertiesNames);

  return res;
}

template<typename Value>
schemas::JWTPair Parse(Value value,
                       USERVER_NAMESPACE::formats::parse::To<schemas::JWTPair>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  schemas::JWTPair res;

  res.access_token =
      value["access_token"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.refresh_token =
      value["refresh_token"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, kschemas_User_PropertiesNames);

  return res;
}

}  // namespace schemas
