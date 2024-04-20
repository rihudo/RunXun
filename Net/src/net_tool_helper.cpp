#include "net_tool_helper.hpp"
#include "log.hpp"
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

NetToolHelper::NetToolHelper() : m_socket(-1)
{}

bool NetToolHelper::network_init(const char* ip, int port)
{
    LOG_INFOR("Start network_init()\n");
    if (!ip || 0 == strlen(ip))
    {
        LOG_ERROR("ip is %s\n", !ip ? "null" : "empty");
        return false;
    }

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > m_socket)
    {
        LOG_ERROR("socket() failed. Return: %d\n", m_socket);
        return false;
    }

    int on = 1;
    if (-1 == setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)))
    {
        LOG_ERROR("setsockopt() failed. Errno:%d\n", errno);
        m_socket = -1;
        return false;
    }

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    int pton_ret = inet_pton(AF_INET, ip, &local_addr.sin_addr);
    if (0 >= pton_ret)
    {
        LOG_ERROR("inet_pton() failed. Return: %d\n", pton_ret);
        m_socket = -1;
        return false;
    }

    int bind_ret = bind(m_socket, (struct sockaddr*)(&local_addr), sizeof(local_addr));
    if (-1 == bind_ret)
    {
        LOG_ERROR("bind() failed. Errno:%d\n", errno);
        m_socket = -1;
        return false;
    }

    LOG_INFOR("End network_init() socket fd:%d\n", m_socket);
    return true;
}

ssize_t NetToolHelper::send_msg(const char* msg, size_t size, struct sockaddr_in* dest_addr)
{
    LOG_INFOR("Start send_msg()\n");
    if (!msg || 0 == size || !dest_addr)
    {
        LOG_ERROR("Can't send msg: args is invalid\n");
        return -1;
    }

    if (!is_valid())
    {
        LOG_ERROR("Can't send msg: NetTool is invalid\n");
        return -1;
    }

    ssize_t send_ret = sendto(m_socket, msg, size, 0, (struct sockaddr*)dest_addr, sizeof(struct sockaddr_in));
    if (-1 == send_ret)
    {
        LOG_ERROR("sendto() failed. Errno:%d\n", errno);
        return -1;
    }
    LOG_INFOR("End send_msg()\n");
    return send_ret;
}

ssize_t NetToolHelper::recv_msg(char* buffer, size_t size, struct sockaddr_in* address)
{
    LOG_INFOR("Start recv_msg()\n");
    if (!buffer || 0 == size)
    {
        LOG_ERROR("Can't recv msg: args is invalid\n");
        return -1;
    }

    if (!is_valid())
    {
        LOG_ERROR("Can't recv msg: NetTool is invalid\n");
        return -1;
    }
    socklen_t address_len = sizeof(struct sockaddr_in);
    ssize_t recv_ret = recvfrom(m_socket, buffer, size, 0, (struct sockaddr*)address, &address_len);
    if (-1 == recv_ret)
    {
        LOG_ERROR("recvfrom() failed. Errno:%d\n", errno);
        return -1;
    }

    LOG_INFOR("End recv_msg()\n");
    return recv_ret;
}

bool NetToolHelper::is_valid()
{
    return m_socket != -1;
}