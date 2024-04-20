#pragma once
#include "message.hpp"

/* 对外提供的能力
 * 1. 广播通信
 * 2. 点对点通信
 * 3. 协议解析
 */
class NetTool
{
public:
    // ip 和 port描述了程序监听的地址.
    // broadcast_port表示发送广播时的目的端口, 若值为0, 则会使用port作为目的端口
    NetTool(const char* ip, int port, int broadcast_port = 0);
    ~NetTool();
    // 此函数中的参数msg中的addr_id不会被使用 Return -1 when failed
    ssize_t broadcast(const Message& msg);
    // Return -1 when failed
    ssize_t send(const Message& msg);
    // Return -1 when failed. Return 0 when no messages are available to be received
    ssize_t recv(Message& msg);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};