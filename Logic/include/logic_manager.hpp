#pragma once
#include <memory>
#include <functional>
#include <list>
#include <string>
#include <vector>
#include <record_entry.hpp>

/* 业务逻辑模块的句柄类
 * 与其他模块的交互均在此发生
 */
class LogicManager
{
public:
    using MSG_CALLBACK = std::function<void(uint32_t, const std::string&)>;

    LogicManager(const std::string& self_name, int port, const std::list<int>& broadcast_port_list);
    ~LogicManager();

    bool start();

    const std::vector<ChatRecordEntry>* get_records(uint32_t uid);

    std::string get_user_name(uint32_t uid);

    // 设置收到hello消息时的处理回调
    bool set_hello_callback(MSG_CALLBACK hello_cb);

    // 设置收到hello_reply消息时的处理回调
    bool set_hello_reply_callback(MSG_CALLBACK hello_reply_cb);

    // 设置收到msg消息时的处理回调
    bool set_msg_callback(MSG_CALLBACK msg_cb);

    // 设置收到msg_reply消息时的处理回调
    bool set_msg_reply_callback(MSG_CALLBACK msg_reply_cb);

    // 设置收到bye消息时的处理回调
    bool set_bye_callback(MSG_CALLBACK bye_cb);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};