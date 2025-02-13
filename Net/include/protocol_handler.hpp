#pragma once
#include <string>
#include <message.hpp>

/* 发送文本的格式
 * |  0~7  |   8~15  | 16~ |
 *   type    msg_len   msg
 */

class ProtocolHandler
{
public:
    std::string generateSendMessage(const Message& msg);
    Message getMessageFromBuffer(const char* buffer, size_t length);
};