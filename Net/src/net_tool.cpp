#include "net_tool.hpp"
#include "net_tool_helper.hpp"
#include <unordered_map>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "protocol_handler.hpp"
#include "log.hpp"
#include "uid_manager.hpp"

#define BROADCAST_IP "255.255.255.255"

class AddrIDHash
{
public:
    std::size_t operator()(const AddrID& addr_id) const
    {
        std::size_t h1 = std::hash<uint32_t>()(std::get<0>(addr_id.getValue()));
        std::size_t h2 = std::hash<uint16_t>()(std::get<1>(addr_id.getValue()));
        return h1 ^ h2;
    }
};

bool operator==(const AddrID& left, const AddrID& right)
{
    return left.getValue() == right.getValue();
}

class NetTool::Impl
{
public:
    Impl(const char* ip, int port, const std::list<int>& broadcast_port) : m_broadcast_port_list(broadcast_port)
    {
        if (m_broadcast_port_list.empty())
        {
            m_broadcast_port_list.push_back(port);
        }
        init_broadcast_addr();
        m_is_ready = m_helper.network_init(ip, port);
    }

    void broadcast_impl(const Message& msg)
    {
        std::string send_msg = m_protocol_handler.generateSendMessage(msg);
        for (auto& addr : m_broadcast_addr_list)
        {
            m_helper.send_msg(send_msg.c_str(), send_msg.size(), &addr);
        }
    }

    ssize_t send_impl(const Message& msg)
    {
        if (0 == m_addr_map.count(msg.addr_id))
        {
            LOG_ERROR("Can't find address by AddrID(%u %u)\n", std::get<0>(msg.addr_id.getValue()), std::get<1>(msg.addr_id.getValue()));
            return -1;
        }

        std::string send_msg = m_protocol_handler.generateSendMessage(msg);
        return m_helper.send_msg(send_msg.c_str(), send_msg.size(), &m_addr_map.at(msg.addr_id));
    }

    ssize_t recv_impl(Message& msg)
    {
        memset(&m_remote_addr, 0, sizeof(m_remote_addr));
        char buffer[1024] = {0};
        ssize_t recv_ret = m_helper.recv_msg(buffer, sizeof(buffer), &m_remote_addr);

        if (m_helper.is_self(m_remote_addr))
        {
            return -2;
        }

        if (0 < recv_ret)
        {
            uint32_t uid = get_next_uid(m_remote_addr.sin_addr.s_addr, m_remote_addr.sin_port);
            if (0 == uid)
            {
                return -3;
            }
            AddrID new_addr(m_remote_addr.sin_addr.s_addr, m_remote_addr.sin_port, uid);
            if (0 == m_addr_map.count(new_addr))
            {
                m_addr_map.emplace(std::make_pair(new_addr, m_remote_addr));
            }
            msg = m_protocol_handler.getMessageFromBuffer(buffer, recv_ret);
            msg.addr_id = new_addr;
        }
        
        return recv_ret;
    }

    bool is_ready_impl()
    {
        return m_is_ready;
    }

private:
    void init_broadcast_addr()
    {
        for (auto& broadcast_port : m_broadcast_port_list)
        {
            struct sockaddr_in temp_addr;
            memset(&temp_addr, 0, sizeof(temp_addr));
            temp_addr.sin_family = AF_INET;
            temp_addr.sin_port = htons(broadcast_port);
            inet_pton(AF_INET, BROADCAST_IP, &temp_addr.sin_addr);
            m_broadcast_addr_list.push_back(temp_addr);
        }
    }

    uint32_t get_next_uid(uint32_t ip, uint16_t port)
    {
        return uid_manager.get_uid(ip, port);
    }

private:
    NetToolHelper m_helper;
    ProtocolHandler m_protocol_handler;
    std::unordered_map<AddrID, struct sockaddr_in, AddrIDHash> m_addr_map;
    struct sockaddr_in m_remote_addr;
    std::list<int> m_broadcast_port_list;
    std::list<struct sockaddr_in> m_broadcast_addr_list;
    bool m_is_ready;
    UidManager uid_manager;
};


NetTool::NetTool(const char* ip, int port, const std::list<int>& broadcast_port) : impl(std::make_unique<Impl>(ip, port, broadcast_port)){}
NetTool::~NetTool() {}

void NetTool::broadcast(const Message& msg)
{
    if (impl)
    {
        impl->broadcast_impl(msg);
    }
}

ssize_t NetTool::send(const Message& msg)
{
    return impl ? impl->send_impl(msg) : -1;
}

ssize_t NetTool::recv(Message& msg)
{
    return impl ? impl->recv_impl(msg) : -1;
}

bool NetTool::is_ready()
{
    return impl && impl->is_ready_impl();
}