#pragma once
#include <string>

class ChatRecordEntry
{
public:
    bool is_self;           // 是否自身发送的
    std::string info;       // 单条聊天记录

public:
    ChatRecordEntry();
    ChatRecordEntry(ChatRecordEntry&& other);
};