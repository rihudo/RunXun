#pragma once
#include <unordered_map>
#include "chat_record.hpp"
#include <sqlite3.h>

/* 数据库聊天记录table:
 * id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, uid INT UNSIGNED, is_self TINYINT, msg TEXT
 */
class RecordManager
{
public:
    RecordManager();
    ~RecordManager();
    void load_user(uint32_t uid);
    bool append_record(uint32_t uid, bool is_self, const std::string& info);
    bool write_back_record(uint32_t uid);
    const std::vector<ChatRecordEntry>* get_records(uint32_t uid);

private:
    void init();

private:
    // 缓存聊天记录 key: 对端uid 
    std::unordered_map<uint32_t, ChatRecord> m_chat_record;
    sqlite3* db;
};