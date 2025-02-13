#include "record_manager.hpp"
#include "log.hpp"

#define DB_PATH "/data/runxun/"
#define DB_NAME "runxun.db"

RecordManager::RecordManager()
{
    init();
}

RecordManager::~RecordManager()
{
    for (auto& kv : m_chat_record)
    {
        kv.second.write_back_new_record();
    }

    if (db)
    {
        sqlite3_close(db);
    }
}

void RecordManager::load_user(uint32_t uid)
{
    if (0 == m_chat_record.count(uid))
    {
        m_chat_record.emplace(std::make_pair(uid, ChatRecord(uid, db)));
    }
}

bool RecordManager::append_record(uint32_t uid, bool is_self, const std::string& info)
{
    return m_chat_record.count(uid) && m_chat_record.at(uid).append_new_chat_message(is_self, info);
}

bool RecordManager::write_back_record(uint32_t uid)
{
    return m_chat_record.count(uid) && m_chat_record.at(uid).write_back_new_record();
}

const std::vector<ChatRecordEntry>* RecordManager::get_records(uint32_t uid)
{
    return m_chat_record.count(uid) ? m_chat_record.at(uid).get_records() : NULL;
}

void RecordManager::init()
{
    LOG_INFOR("Start RecordManager::init()\n");
    if (SQLITE_OK != sqlite3_open(DB_PATH DB_NAME, &db))
    {
        LOG_ERROR("Create/open db failed:%s\n", sqlite3_errmsg(db));
        db = nullptr;
        return;
    }

    // 检查table是否已经存在
    const char* check_table_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='" RECORD_TABLE_NAME "';";
    sqlite3_stmt* stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, check_table_sql, -1, &stmt, NULL))
    {
        LOG_ERROR("Checkout table:" RECORD_TABLE_NAME "fialed\n");
        sqlite3_close(db);
        db = nullptr;
        return;
    }

    // table不存在, 现在来创建table
    if (SQLITE_ROW != sqlite3_step(stmt))
    {
        const char* create_talbe_sql = "CREATE TABLE " RECORD_TABLE_NAME "(id INTEGER PRIMARY KEY AUTOINCREMENT, uid INT UNSIGNED, is_self TINYINT, msg TEXT);";
        if (SQLITE_OK != sqlite3_exec(db, create_talbe_sql, NULL, NULL, NULL))
        {
            LOG_ERROR("Create table " RECORD_TABLE_NAME " failed\n");
            sqlite3_close(db);
            db = nullptr;
            return;
        }
    }

    sqlite3_finalize(stmt);
    LOG_INFOR("End RecordManager::init()\n");
}