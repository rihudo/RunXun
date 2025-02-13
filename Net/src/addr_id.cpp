#include "addr_id.hpp"

class AddrID::Impl
{
public:
    Impl() : m_addr(INVALID_ADDR_ID), m_port(INVALID_PORT_ID), m_uid(INVALID_UID)
    {}

    Impl(uint32_t _addr, uint16_t _port, uint32_t uid) : m_addr(_addr), m_port(_port), m_uid(uid)
    {}

    std::pair<uint32_t, uint16_t> getValueImpl() const
    {
        return {m_addr, m_port};
    }

    uint32_t get_uid_impl() const
    {
        return m_uid;
    }

private:
    uint32_t m_addr;    // in_addr_t
    uint16_t m_port;    // in_port_t
    uint32_t m_uid;
};

AddrID::AddrID() : impl(std::make_shared<Impl>())
{}

AddrID::AddrID(uint32_t _addr, uint16_t _port, uint32_t _uid) : impl(std::make_shared<Impl>(_addr, _port, _uid))
{}

AddrID::~AddrID()
{}

uint32_t AddrID::get_uid() const
{
    return impl ? impl->get_uid_impl() : INVALID_UID;
}

std::pair<uint32_t, uint16_t> AddrID::getValue() const
{
    return impl ? impl->getValueImpl() : std::pair<uint32_t, uint16_t>{INVALID_ADDR_ID, INVALID_PORT_ID};
}