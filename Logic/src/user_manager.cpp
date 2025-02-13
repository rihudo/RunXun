#include "user_manager.hpp"
#include "log.hpp"

#define USER_TABLE "user_table"
#define DB_PATH "/data/runxun/"
#define DB_NAME "runxun.db"

UserManager::UserManager()
{
    init_name();
}

UserManager::~UserManager()
{
    if (is_valid())
    {
        write_back_new_name();
        sqlite3_close(db);
    }
}

std::string UserManager::get_name(uint32_t uid)
{
    if (existed_name_map.count(uid))
    {
        return existed_name_map.at(uid);
    }
    return new_name_map.at(uid);
}

void UserManager::set_name(uint32_t uid, const std::string& name)
{
    if (0 == online_user.count(uid))
    {
        online_user.emplace(uid);
    }

    if (existed_name_map.count(uid) || new_name_map.count(uid))
    {
        return;
    }

    new_name_map.emplace(std::make_pair(uid, name));
}

void UserManager::unset_user(uint32_t uid)
{
    if (online_user.count(uid))
    {
        online_user.erase(uid);
    }
}

void UserManager::init_name()
{
    LOG_INFOR("Start init_name()\n");
    if (SQLITE_OK != sqlite3_open(DB_PATH DB_NAME, &db))
    {
        LOG_ERROR("Create/open db failed:%s\n", sqlite3_errmsg(db));
        db = nullptr;
        return;
    }

    // 查询表格是否已经存在
    const char* check_table_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='" USER_TABLE "';";
    sqlite3_stmt* stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, check_table_sql, -1, &stmt, NULL))
    {
        LOG_INFOR("init_name failed: prepare\n");
        sqlite3_close(db);
        db = nullptr;
        return;
    }

    // 表格不存在, 创建表格
    if (SQLITE_ROW != sqlite3_step(stmt))
    {
        const char* create_user_table_sql = "CREATE TABLE " USER_TABLE " (uid INT PRIMARY KEY, name TEXT);";
        if (SQLITE_OK != sqlite3_exec(db, create_user_table_sql, NULL, NULL, NULL))
        {
            LOG_INFOR("init_name failed: create table\n");
            sqlite3_close(db);
            db = nullptr;
            return;
        }
    }
    // 表格已经存在, 查询已有记录
    else
    {
        sqlite3_finalize(stmt);
        const char* query_sql = "SELECT uid,name from " USER_TABLE ";";
        if (SQLITE_OK != sqlite3_prepare_v2(db, query_sql, -1, &stmt, NULL))
        {
            LOG_ERROR("Query user info failed\n");
            return;
        }

        while (SQLITE_ROW == sqlite3_step(stmt))
        {
            uint32_t uid = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
            const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            LOG_INFOR("Existed user: %u %s\n", uid, name);
            existed_name_map.emplace(std::make_pair(uid, std::string(name)));
        }
    }

    sqlite3_finalize(stmt);
    LOG_INFOR("End init_name()\n");
}

bool UserManager::is_valid()
{
    return db != nullptr;
}

void UserManager::write_back_new_name()
{
    const char* write_new_info = "INSERT INTO " USER_TABLE " VALUES(?,?)";
    sqlite3_stmt* stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, write_new_info, -1, &stmt, NULL))
    {
        LOG_ERROR("write_back_new_name() failed: prepare\n");
        return;
    }
    uint32_t successed_count = 0;
    for (auto& kv : new_name_map)
    {
        if (SQLITE_OK == sqlite3_bind_int(stmt, 1, kv.first)
            && SQLITE_OK == sqlite3_bind_text(stmt, 2, kv.second.c_str(), -1, NULL)
            && SQLITE_DONE == sqlite3_step(stmt))
        {
            ++successed_count;
        }
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    LOG_INFOR("Write back new info: total:%lu successed:%u\n", new_name_map.size(), successed_count);
    new_name_map.clear();
}