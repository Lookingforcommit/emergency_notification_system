#pragma once

#include <userver/telegram/bot/requests/request.hpp>
#include <userver/telegram/bot/types/chat_id.hpp>
#include <userver/telegram/bot/types/input_file.hpp>
#include <userver/telegram/bot/types/message_entity.hpp>
#include <userver/telegram/bot/types/message.hpp>
#include <userver/telegram/bot/types/reply_markup.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "userver/formats/json_fwd.hpp"

USERVER_NAMESPACE_BEGIN

namespace telegram::bot {

/// @brief Use this method to send audio files, if you want Telegram clients
/// to display them in the music player.
/// @note For sending voice messages, use the sendVoice method instead.
/// @see https://core.telegram.org/bots/api#sendaudio
struct SendAudioMethod {
  static constexpr std::string_view kName = "sendAudio";

  static constexpr auto kHttpMethod = clients::http::HttpMethod::kPost;

  struct Parameters{
    using Audio = std::variant<InputFile, std::string>;

    Parameters(ChatId _chat_id, Audio _audio);

    /// @brief Unique identifier for the target chat or username of the
    /// target channel (in the format @channelusername).
    ChatId chat_id;

    /// @brief Unique identifier for the target message thread (topic) of
    /// the forum.
    /// @note For forum supergroups only,
    std::optional<std::int64_t> message_thread_id;

    /// @brief Audio file to send.
    /// 1. Pass a file_id as std::string to send a audio that exists on the
    /// Telegram servers (recommended)
    /// 2. Pass an HTTP URL as a std::string to get a audio from the
    /// Internet.
    /// 3. Pass InputFile to upload a new audio.
    /// @note Your audio must be in the .MP3 or .M4A format.
    /// @note Bots can currently send audio files of up to 50 MB in size, this
    /// limit may be changed in the future.
    /// @see https://core.telegram.org/bots/api#sending-files
    Audio audio;

    /// @brief Audio caption, 0-1024 characters after entities parsing.
    std::optional<std::string> caption;

    /// @brief Mode for parsing entities in the animation caption.
    /// @see https://core.telegram.org/bots/api#formatting-options
    /// for more details.
    std::optional<std::string> parse_mode;

    /// @brief List of special entities that appear in the caption, which can
    // be specified instead of parse_mode.
    std::optional<std::vector<MessageEntity>> caption_entities;

    /// @brief Duration of the audio in seconds.
    std::optional<std::chrono::seconds> duration;

    /// @brief Performer.
    std::optional<std::string> performer;

    /// @brief Track name.
    std::optional<std::string> title;

    /// @brief Thumbnail of the file sent;
    /// @note Can be ignored if thumbnail generation for the file is supported
    /// server-side.
    /// @note The thumbnail should be in JPEG format and less than 200 kB in
    /// size. A thumbnail's width and height should not exceed 320. 
    /// @note Thumbnails can't be reused and can be only uploaded as a new file.
    /// @see https://core.telegram.org/bots/api#sending-files
    std::unique_ptr<InputFile> thumbnail;

    /// @brief Sends the message silently. Users will receive a notification
    /// with no sound.
    std::optional<bool> disable_notification;

    /// @brief Protects the contents of the sent message from forwarding
    /// and saving.
    std::optional<bool> protect_content;

    /// @brief If the message is a reply, ID of the original message.
    std::optional<std::int64_t> reply_to_message_id;

    /// @brief Pass True if the message should be sent even if the specified
    /// replied-to message is not found.
    std::optional<bool> allow_sending_without_reply;

    /// @brief Additional interface options.
    std::optional<ReplyMarkup> reply_markup;
  };

  /// @brief On success, the sent Message is returned.
  using Reply = Message;

  static void FillRequestData(clients::http::Request& request,
                              const Parameters& parameters);

  static Reply ParseResponseData(clients::http::Response& response);
};

SendAudioMethod::Parameters Parse(
    const formats::json::Value& json,
    formats::parse::To<SendAudioMethod::Parameters>);

formats::json::Value Serialize(const SendAudioMethod::Parameters& parameters,
                               formats::serialize::To<formats::json::Value>);

using SendAudioRequest = Request<SendAudioMethod>;

}  // namespace telegram::bot

USERVER_NAMESPACE_END
