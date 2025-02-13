#pragma once
#include <cstdint>
#include <list>
#include "message.hpp"

#define RECV_FAILED -1
#define RECV_SELF -2
#define RECV_INVALID_UID -3

/* 对外提供的能力
 * 1. 广播通信
 * 2. 点对点通信
 * 3. 协议解析
 */
class NetTool
{
public:
    // ip 和 port描述了程序监听的地址.
    // broadcast_port表示发送广播时的目的端口, 若为空, 则会使用port作为目的端口
    NetTool(const char* ip, int port, const std::list<int>& broadcast_port);
    ~NetTool();
    // 此函数中的参数msg中的addr_id不会被使用
    void broadcast(const Message& msg);
    // Return -1 when failed
    ssize_t send(const Message& msg);
    /* Return 0 when no messages are available to be received.
     * Return -1 when failed.
     * Return -2 when receiving broadcast data sent by itself.
     * Return -3 位该地址分配UID失败了
     */
    ssize_t recv(Message& msg);

    bool is_ready();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};