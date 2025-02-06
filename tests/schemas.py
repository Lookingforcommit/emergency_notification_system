from voluptuous import Schema, Required, Optional

JwtSchema = Schema({
    Required("access_token"): str,
    Required("refresh_token"): str,
})

BaseTemplateSchema = Schema({
    Required("name"): str,
    Optional("message_text"): str,
})

ReturnedTemplateSchema = BaseTemplateSchema.extend({
    Required("master_id"): str
})

TemplateWithoutIdSchema = BaseTemplateSchema

TemplateDraftSchema = ReturnedTemplateSchema.extend({
    Required("draft_id"): str
})

TemplateWithIdSchema = ReturnedTemplateSchema.extend({
    Required("notification_template_id"): str
})

TemplateWithIdListSchema = Schema([TemplateWithIdSchema])

BaseRecipientSchema = Schema({
    Required("name"): str,
    Optional("email"): str,
    Optional("phone_number"): str,
    Optional("telegram_id"): int,
})

ReturnedRecipientSchema = BaseRecipientSchema.extend({
    Required("master_id"): str
})

RecipientWithoutIdSchema = BaseRecipientSchema

RecipientDraftSchema = ReturnedRecipientSchema.extend({
    Required("draft_id"): str
})

RecipientWithIdSchema = ReturnedRecipientSchema.extend({
    Required("recipient_id"): str
})

RecipientWithIdListSchema = Schema([RecipientWithIdSchema])

BaseGroupSchema = Schema({
    Required("name"): str,
    Optional("notification_template_id"): str,
    Required("active"): bool,
})

ReturnedGroupSchema = BaseGroupSchema.extend({
    Required("master_id"): str
})

GroupWithoutIdSchema = BaseGroupSchema

GroupDraftSchema = ReturnedGroupSchema.extend({
    Required("draft_id"): str
})

GroupWithIdSchema = ReturnedGroupSchema.extend({
    Required("recipient_group_id"): str
})

GroupWithIdListSchema = Schema([GroupWithIdSchema])
