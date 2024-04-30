#pragma once
#include <sqlite3.h>
#include <unordered_map>

class UidManager
{
public:
    UidManager();
    ~UidManager();
    /* 如果已经存在则返回既有uid
     * 如果不存在则分配一个新的uid, 并插入到数据库中
     * return 0 when failed
     */
    uint32_t get_uid(uint32_t ip, uint16_t port);

private:
    void init();
    int mkdir_recursive(const char* path);
    bool is_valid();

private:
    sqlite3* db;
    std::unordered_map<std::string, uint16_t> cached_uid;
};