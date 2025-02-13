#include <string.h>
#include "protocol_handler.hpp"
#include "log.hpp"

std::string ProtocolHandler::generateSendMessage(const Message& msg)
{
    std::string result;
    result.push_back((char)msg.msg_type);
    auto info_size = msg.info.size();
    char info_size_str[2] = {0};
    memcpy(info_size_str, &info_size, sizeof(info_size_str));
    result.push_back(info_size_str[0]);
    result.push_back(info_size_str[1]);
    result.append(msg.info);
    return result;
}

Message ProtocolHandler::getMessageFromBuffer(const char* buffer, size_t length)
{
    Message msg;
    msg.msg_type = MSG_TYPE::invalid_type;
    if (!buffer)
    {
        LOG_ERROR("buffer is null\n");
        return msg;
    }

    if (3 <= length)
    {
        uint16_t info_len = 0;
        memcpy(&info_len, &buffer[1], sizeof(info_len));
        if (length - 3 != info_len)
        {
            LOG_ERROR("Info length is invalid\n");
            return msg;
        }
        msg.info = &buffer[3];
        msg.msg_type = (MSG_TYPE)buffer[0];
    }
    return msg;
}