#include <list>
#include <unordered_map>
#include "logic_manager.hpp"
#include "net_tool.hpp"
#include "msg_listener.hpp"
#include <cassert>

class LogicManager::Impl
{
public:
    Impl(const std::string& self_name, int port, const std::list<int>& broadcast_port_list) :
        m_self_name(self_name),
        m_net_tool("0.0.0.0", port, broadcast_port_list),
        m_listener(m_callback_map, m_net_tool, m_user_manager, m_record_manager, m_self_name)
    {
        assert(m_net_tool.is_ready());
    }

    bool start_impl()
    {
        if (m_listener.start_listen())
        {
            say_hello(m_self_name);
            return true;
        }
        return false;
    }

    void set_callback(MSG_TYPE type, MSG_CALLBACK cb)
    {
        m_callback_map[type] = std::move(cb);
    }

    const std::vector<ChatRecordEntry>* get_records_impl(uint32_t uid)
    {
        return m_record_manager.get_records(uid);
    }

    std::string get_user_name_impl(uint32_t uid)
    {
        return m_user_manager.get_name(uid);
    }

private:
    void say_hello(const std::string& self_name)
    {
        Message hello_msg;
        hello_msg.msg_type = MSG_TYPE::hello;
        hello_msg.info = self_name;
        m_net_tool.broadcast(hello_msg);
    }

private:
    std::string m_self_name;
    NetTool m_net_tool;
    std::unordered_map<MSG_TYPE, MSG_CALLBACK> m_callback_map;
    UserManager m_user_manager;
    RecordManager m_record_manager;
    MsgListener m_listener;
};


LogicManager::LogicManager(const std::string& self_name, int port, const std::list<int>& broadcast_port_list) :
    impl(std::make_unique<Impl>(self_name, port, broadcast_port_list))
{}

LogicManager::~LogicManager()
{}

bool LogicManager::start()
{
    return impl && impl->start_impl();
}

std::string LogicManager::get_user_name(uint32_t uid)
{
    return impl ? impl->get_user_name_impl(uid) : "";
}

const std::vector<ChatRecordEntry>* LogicManager::get_records(uint32_t uid)
{
    return impl ? impl->get_records_impl(uid) : nullptr;
}

#define SET_CALLBACK(type, cb)          \
    if (impl)                           \
    {                                   \
        impl->set_callback(type, cb);   \
        return true;                    \
    }                                   \
    return false                        \

bool LogicManager::set_hello_callback(MSG_CALLBACK hello_cb)
{
    SET_CALLBACK(MSG_TYPE::hello, hello_cb);
}

bool LogicManager::set_hello_reply_callback(MSG_CALLBACK hello_reply_cb)
{
    SET_CALLBACK(MSG_TYPE::hello_reply, hello_reply_cb);
}

bool LogicManager::set_msg_callback(MSG_CALLBACK msg_cb)
{
    SET_CALLBACK(MSG_TYPE::msg, msg_cb);
}

bool LogicManager::set_msg_reply_callback(MSG_CALLBACK msg_reply_cb)
{
    SET_CALLBACK(MSG_TYPE::msg_reply, msg_reply_cb);
}

bool LogicManager::set_bye_callback(MSG_CALLBACK bye_cb)
{
    SET_CALLBACK(MSG_TYPE::bye, bye_cb);
}