#pragma once
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <string>
#include <sqlite3.h>

class UserManager
{
public:
    UserManager();
    ~UserManager();

    /**
     * @brief 查询用户昵称
     * 
     * @param uid 用户ID
     * @return std::string 空表示没有查询到
     */
    std::string get_name(uint32_t uid);

    void set_name(uint32_t uid, const std::string& name);
    
    void unset_user(uint32_t uid);

    const std::unordered_map<uint32_t, std::string>* get_existed_name_map() const;

    const std::unordered_map<uint32_t, std::string>* get_new_name_map() const;

    const std::unordered_set<uint32_t>* get_online_user() const;

private:
    void init_name();

    bool is_valid();

    // 将新增的用户昵称回写到数据库
    void write_back_new_name();
    
private:
    sqlite3* db;
    // key: uid value: 用户昵称
    std::unordered_map<uint32_t, std::string> existed_name_map;
    std::unordered_map<uint32_t, std::string> new_name_map;
    // 在线的用户
    std::unordered_set<uint32_t> online_user;
};