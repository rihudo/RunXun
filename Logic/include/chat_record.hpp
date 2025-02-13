#pragma once
#include <string>
#include <cstdint>
#include <sqlite3.h>
#include <vector>
#include "record_entry.hpp"

#define RECORD_TABLE_NAME "chat_record_table"

class ChatRecord
{
public:
    ChatRecord(uint32_t uid, sqlite3* _db);
    ChatRecord(ChatRecord&& other);
    // 将新产生的聊天记录写回到数据库
    bool write_back_new_record();
    // 写入一条聊天记录
    bool append_new_chat_message(bool is_self, const std::string& info);
    const std::vector<ChatRecordEntry>* get_records();

private:
    bool is_valid();
    void get_existed_record(uint32_t uid);

private:
    sqlite3* db;
    size_t existed_idx;     // 已有聊天记录的下标: 不包括该下标指向的指
    uint32_t m_uid;
    std::vector<ChatRecordEntry> records; // 聊天记录
};