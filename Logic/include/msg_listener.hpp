#pragma once
#include <thread>
#include <memory>
#include <unordered_map>
#include <functional>
#include <net_tool.hpp>
#include "user_manager.hpp"
#include "record_manager.hpp"

class MsgListener
{
public:
    using CB_MAP = std::unordered_map<MSG_TYPE, std::function<void(uint32_t, const std::string&)>>;
    MsgListener(const CB_MAP& cb_map, NetTool& net_tool, UserManager& user_manager, RecordManager& record_manager, const std::string& m_self_name);
    bool start_listen();

private:
    void dispatch_msg(const Message& msg);
    void handle_hello(const Message& msg);
    void handle_hello_reply(const Message& msg);
    void handle_msg(const Message& msg);
    void handle_msg_reply(const Message& msg);
    void handle_bye(const Message& msg);
    void listen_func();

private:
    std::unique_ptr<std::thread> m_listen_thread;
    const CB_MAP& m_cb_map;
    NetTool& m_net_tool;
    UserManager& m_user_manager;
    RecordManager& m_record_manager;
    const std::string& m_self_name;
};