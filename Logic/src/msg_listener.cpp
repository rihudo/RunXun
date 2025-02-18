#include "msg_listener.hpp"
#include "log.hpp"

MsgListener::MsgListener(const CB_MAP& cb_map, NetTool& net_tool, UserManager& user_manager,  RecordManager& record_manager, const std::string& self_name) :
    m_cb_map(cb_map),
    m_net_tool(net_tool),
    m_user_manager(user_manager),
    m_record_manager(record_manager),
    m_self_name(self_name)
{}

bool MsgListener::start_listen()
{
    m_listen_thread = std::make_unique<std::thread>([this]{this->listen_func();});
    m_listen_thread->detach();
    return true;
}

void MsgListener::dispatch_msg(const Message& msg)
{
    switch (msg.msg_type)
    {
    case MSG_TYPE::hello:
        handle_hello(msg);
        break;

    case MSG_TYPE::hello_reply:
        handle_hello_reply(msg);
        break;
    
    case MSG_TYPE::msg:
        handle_msg(msg);
        break;

    case MSG_TYPE::msg_reply:
        handle_msg_reply(msg);
        break;
    
    case MSG_TYPE::bye:
        handle_bye(msg);
        break;

    default:
        LOG_ERROR("Unknow msg type:%d\n", msg.msg_type);
        break;
    }
}

void MsgListener::handle_hello(const Message& msg)
{
    uint32_t uid = msg.addr_id.get_uid();
    if (0 == uid)
    {
        LOG_ERROR("Handle_hello uid is 0\n");
        return;
    }
    LOG_INFOR("Hello msg from uid:%u\n", uid);
    // 维护用户信息
    m_user_manager.set_name(uid, msg.info);
    // 加载聊天记录
    m_record_manager.load_user(uid);
    // 回复msg reply
    Message hello_reply_msg;
    hello_reply_msg.msg_type = MSG_TYPE::hello_reply;
    hello_reply_msg.info = m_self_name; //带上自身昵称
    hello_reply_msg.addr_id = msg.addr_id;
    m_net_tool.send(hello_reply_msg);
}

void MsgListener::handle_hello_reply(const Message& msg)
{
    uint32_t uid = msg.addr_id.get_uid();
    if (0 == uid)
    {
        LOG_ERROR("handle_hello_reply uid is 0\n");
        return;
    }
    LOG_INFOR("Hello reply msg from uid:%u\n", uid);
    // 维护用户信息
    m_user_manager.set_name(uid, msg.info);
    // 加载聊天记录
    m_record_manager.load_user(uid);
}

void MsgListener::handle_msg(const Message& msg)
{
    uint32_t uid = msg.addr_id.get_uid();
    if (0 == uid)
    {
        LOG_ERROR("handle_msg uid is 0\n");
        return;
    }
    LOG_INFOR("Got msg: %s from uid:%u\n", msg.info.c_str(), uid);
    // 添加聊天记录
    m_record_manager.append_record(uid, false, msg.info);
    // 发送回复
    Message msg_reply_msg;
    msg_reply_msg.msg_type = MSG_TYPE::msg_reply;
    msg_reply_msg.addr_id = msg.addr_id;
    m_net_tool.send(msg_reply_msg);
}

void MsgListener::handle_msg_reply(const Message& msg)
{
    // do notion
    (void)msg;
}

void MsgListener::handle_bye(const Message& msg)
{
    // 维护用户信息
    uint32_t uid = msg.addr_id.get_uid();
    if (0 == uid)
    {
        LOG_ERROR("handle_bye uid is 0\n");
        return;
    }
    m_user_manager.unset_user(uid);
}

void MsgListener::listen_func()
{
    LOG_INFOR("Start listen\n");
    for (;;)
    {
        Message msg;
        auto ret = m_net_tool.recv(msg);
        LOG_INFOR("Recv ret:%ld\n",ret);
        if (RECV_SELF == ret)
        {
            LOG_INFOR("Skip self broadcast\n");
            continue;
        }

        if (RECV_FAILED == ret)
        {
            LOG_ERROR("Recv failed\n");
            continue;
        }

        if (0 < ret)
        {
            dispatch_msg(msg);
            // 调回上层设置的回调函数
            if (m_cb_map.count(msg.msg_type) && m_cb_map.at(msg.msg_type))
            {
                m_cb_map.at(msg.msg_type)(msg.addr_id.get_uid(), msg.info);
            }
        }
    }
}