#pragma once
#include <sys/socket.h>

class NetToolHelper
{
public:
    NetToolHelper();
    bool network_init(const char* ip, int port);
    // Return -1 when failed
    ssize_t send_msg(const char* msg, size_t size, struct sockaddr_in* dest_addr);
    // Return -1 when failed. Return 0 when no messages are available to be received
    ssize_t recv_msg(char* buffer, size_t size, struct sockaddr_in* address);

private:
    bool is_valid();

private:
    int m_socket;
};