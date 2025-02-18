#include "chat_record.hpp"
#include "log.hpp"


#define IS_SELF_TRUE 1
#define IS_SELF_FALSE 0

ChatRecord::ChatRecord(uint32_t uid, sqlite3* _db) : m_uid(uid), existed_idx(0), db(_db)
{
    if (is_valid())
    {
        get_existed_record(uid);
    }
}

ChatRecord::ChatRecord(ChatRecord&& other) :
    db(other.db),
    existed_idx(other.existed_idx),
    m_uid(other.m_uid),
    records(std::move(other.records))
{
    other.db = NULL;
}

bool ChatRecord::write_back_new_record()
{
    if (existed_idx >= records.size())
    {
        LOG_INFOR("No chat record need write back\n");
        return true;
    }
    
    LOG_INFOR("Start write_back_new_record()\n");
    if (!is_valid())
    {
        LOG_ERROR("write_back_new_record() db is invalid\n");
        return false;
    }

    const char* write_sql = "INSERT INTO " RECORD_TABLE_NAME " VALUES(NULL,?,?,?);";
    sqlite3_stmt* stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, write_sql, -1, &stmt, NULL))
    {
        LOG_ERROR("Write back failed:prepare\n");
        return false;
    }
    uint32_t failed_count = 0, successed_count = 0;
    for (; existed_idx < records.size(); ++existed_idx)
    {
        int is_self_value = records[existed_idx].is_self ? IS_SELF_TRUE : IS_SELF_FALSE;
        if (SQLITE_OK == sqlite3_bind_int(stmt, 1, m_uid)
            && SQLITE_OK == sqlite3_bind_int(stmt, 2, is_self_value)
            && SQLITE_OK == sqlite3_bind_text(stmt, 3, records[existed_idx].info.c_str(), -1, NULL)
            && SQLITE_DONE == sqlite3_step(stmt)
            )
        {
            ++successed_count;
        }
        else
        {
            ++failed_count;
        }
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    LOG_INFOR("End write_back_new_record() successed:%u failed:%u\n", successed_count, failed_count);
    return true;
}

bool ChatRecord::append_new_chat_message(bool is_self, const std::string& info)
{
    if (!is_valid())
    {
        LOG_ERROR("append_new_chat_message_impl() db is invalid\n");
        return false;
    }
    ChatRecordEntry new_record;
    new_record.is_self = is_self;
    new_record.info = info;
    records.emplace_back(std::move(new_record));
    return true;
}

const std::vector<ChatRecordEntry>* ChatRecord::get_records()
{
    return &records;    
}

bool ChatRecord::is_valid()
{
    return nullptr != db;
}

void ChatRecord::get_existed_record(uint32_t uid)
{
    LOG_INFOR("Start get_existed_record()\n");
    const char* query_sql = "SELECT is_self, msg FROM " RECORD_TABLE_NAME " WHERE uid = ?;";
    sqlite3_stmt* stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, query_sql, -1, &stmt, NULL))
    {
        LOG_ERROR("Query record failed: prepare\n");
        return;
    }

    if (SQLITE_OK != sqlite3_bind_int(stmt, 1, m_uid))
    {
        LOG_ERROR("Query record failed: bind\n");
        sqlite3_finalize(stmt);
        return;
    }
    uint32_t record_count = 0;
    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        ChatRecordEntry entry;
        entry.is_self = IS_SELF_TRUE == sqlite3_column_int(stmt, 0);
        entry.info = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        records.emplace_back(std::move(entry));
        ++record_count;
        ++existed_idx;
    }
    LOG_INFOR("Found %u records\n", record_count);
    sqlite3_finalize(stmt);
    LOG_INFOR("End get_existed_record()\n");
}