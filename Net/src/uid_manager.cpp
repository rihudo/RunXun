#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>
#include "uid_manager.hpp"
#include "log.hpp"

#define DB_PATH "/data/runxun/"
#define DB_NAME "runxun.db"
#define UID_TABLE_NAME "uid_table"

UidManager::UidManager() : db(nullptr), max_uid(0)
{
    init();
}

UidManager::~UidManager()
{
    if (db)
    {
        sqlite3_close(db);
    }
}

uint32_t UidManager::get_uid(uint32_t ip, uint16_t port)
{
    if (!is_valid())
    {
        return 0;
    }

    char ip_info[16] = {0};
    snprintf(ip_info, sizeof(ip_info), "%u%u", ip, port);
    LOG_INFOR("UidManager::get_uid ip_info:%s\n", ip_info);
    // 先查询缓存
    if (cached_uid.count(ip_info))
    {
        LOG_INFOR("Cached uid:%u for %s\n", cached_uid.at(ip_info), ip_info);
        return cached_uid.at(ip_info);
    }

    sqlite3_stmt *stmt;
    // 缓存中没有, 分配新UID.
    LOG_INFOR("Max uid: %u new uid:%u\n", max_uid, max_uid + 1);
    cached_uid[ip_info] = ++max_uid;

    // 将新分配的UID插入到数据库中
    const char* insert_new_uid_sql = "INSERT INTO " UID_TABLE_NAME " VALUES(?,?);";
    if (SQLITE_OK != sqlite3_prepare_v2(db, insert_new_uid_sql, -1, &stmt, NULL))
    {
        LOG_ERROR("Insert new uid fialed(prepare)!!!\n");
        return 0;
    }

    if (SQLITE_OK != sqlite3_bind_text(stmt, 1, ip_info, -1, NULL) || SQLITE_OK != sqlite3_bind_int(stmt, 2, max_uid))
    {
        LOG_ERROR("Insert new uid fialed(bind)!!!\n");
        return 0;
    }

    if (SQLITE_DONE != sqlite3_step(stmt))
    {
        LOG_ERROR("Insert new uid fialed(step)!!!\n");
        return 0;
    }

    sqlite3_finalize(stmt);

    return max_uid;
}

void UidManager::init()
{
    LOG_INFOR("Start UidManager::init()\n");
    if (0 != mkdir_recursive(DB_PATH))
    {
        LOG_ERROR("Failed to make db path:%s errno:%d\n", DB_PATH, errno);
        return;
    }

    if (SQLITE_OK != sqlite3_open(DB_PATH DB_NAME, &db))
    {
        LOG_ERROR("Create/open db failed:%s\n", sqlite3_errmsg(db));
        db = nullptr;
        return;
    }

    // 检查table是否已经存在
    const char* check_table_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='" UID_TABLE_NAME "';";
    sqlite3_stmt* stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, check_table_sql, -1, &stmt, NULL))
    {
        LOG_ERROR("Checkout table:" UID_TABLE_NAME "fialed\n");
        sqlite3_close(db);
        db = nullptr;
        return;
    }

    // table不存在, 现在来创建table
    if (SQLITE_ROW != sqlite3_step(stmt))
    {
        const char* create_talbe_sql = "CREATE TABLE " UID_TABLE_NAME " (ip_info VARCHAR(16) PRIMARY KEY, uid INT UNSIGNED);";
        if (SQLITE_OK != sqlite3_exec(db, create_talbe_sql, NULL, NULL, NULL))
        {
            LOG_ERROR("Create table " UID_TABLE_NAME " failed\n");
            sqlite3_close(db);
            db = nullptr;
            return;
        }
    }
    // table已经存在则, 查询已有的uid记录
    else
    {
        sqlite3_finalize(stmt);
        const char* query_existed_uid_sql = "SELECT ip_info,uid from " UID_TABLE_NAME ";";
        if (SQLITE_OK != sqlite3_prepare_v2(db, query_existed_uid_sql, -1, &stmt, NULL))
        {
            LOG_ERROR("Query existed uid failed:prepare\n");
        }
        else
        {
            while (SQLITE_ROW == sqlite3_step(stmt))
            {
                const char* ip_info = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                uint32_t existed_uid = static_cast<uint32_t>(sqlite3_column_int(stmt, 1));
                LOG_INFOR("Existed uid:%u for %s\n", existed_uid, ip_info);
                cached_uid[ip_info] = existed_uid;
                max_uid = existed_uid > max_uid ? existed_uid : max_uid;
            }
        }
    }

    sqlite3_finalize(stmt);

    LOG_INFOR("End UidManager::init()\n");
}

int UidManager::mkdir_recursive(const char* path)
{
    struct stat st;
    char* path_copy = strdup(path);
    int status = 0;
    if (0 != stat(path, &st))
    {
        if (0 != mkdir_recursive(dirname(path_copy)))
        {
            status = -1;
        }
        else if (0 != mkdir(path, 0775))
        {
            status = -1;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        status = -1;
    }
    free(path_copy);
    return status;
}

bool UidManager::is_valid()
{
    if (!db)
    {
        LOG_ERROR("db is invalid !!!\n");
    }
    return db != nullptr;
}