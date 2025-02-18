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

    ~Impl()
    {
        say_bye();
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

    ssize_t send_msg_impl(uint32_t uid, const char* msg)
    {
        ssize_t ret = m_net_tool.send(uid, msg);
        if (0 < ret)
        {
            m_record_manager.append_record(uid, true, msg);
        }
        return ret;
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

    void load_user_impl(int32_t uid)
    {
        return m_record_manager.load_user(uid);
    }

    const std::unordered_map<uint32_t, std::string>* get_existed_name_map_impl() const
    {
        return m_user_manager.get_existed_name_map();
    }

    const std::unordered_map<uint32_t, std::string>* get_new_name_map_impl() const
    {
        return m_user_manager.get_new_name_map();
    }

    const std::unordered_set<uint32_t>* get_online_user_impl() const
    {
        return m_user_manager.get_online_user();
    }

private:
    void say_hello(const std::string& self_name)
    {
        Message hello_msg;
        hello_msg.msg_type = MSG_TYPE::hello;
        hello_msg.info = self_name;
        m_net_tool.broadcast(hello_msg);
    }

    void say_bye()
    {
        Message bye_msg;
        bye_msg.msg_type = MSG_TYPE::bye;
        m_net_tool.broadcast(bye_msg);
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

void LogicManager::load_user(int32_t uid)
{
    if (impl)
    {
        impl->load_user_impl(uid);
    }
}

ssize_t LogicManager::send_msg(uint32_t uid, const char* msg)
{
    return impl ? impl->send_msg_impl(uid, msg) : -1;
}

const std::unordered_map<uint32_t, std::string>* LogicManager::get_existed_name_map() const
{
    return impl ? impl->get_existed_name_map_impl() : nullptr;
}

const std::unordered_map<uint32_t, std::string>* LogicManager::get_new_name_map() const
{
    return impl ? impl->get_new_name_map_impl() : nullptr;
}

const std::unordered_set<uint32_t>* LogicManager::get_online_user() const
{
    return impl ? impl->get_online_user_impl() : nullptr;
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