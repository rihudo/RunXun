#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdint>
#include <unordered_set>

class NetToolHelper
{
public:
    NetToolHelper();
    ~NetToolHelper();
    bool network_init(const char* ip, int port);
    // Return -1 when failed
    ssize_t send_msg(const char* msg, size_t size, struct sockaddr_in* dest_addr);
    // Return -1 when failed. Return 0 when no messages are available to be received
    ssize_t recv_msg(char* buffer, size_t size, struct sockaddr_in* address);
    // 判断是否是自身的地址
    bool is_self(struct sockaddr_in& addr);

private:
    bool is_valid();
    void get_all_local_ip();

private:
    int m_socket;
    std::unordered_set<uint32_t> m_local_ip_set;
    uint16_t m_binded_port;
};