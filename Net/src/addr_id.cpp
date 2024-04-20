#include "addr_id.hpp"

class AddrID::Impl
{
public:
    Impl() : m_addr(INVALID_ADDR_ID), m_port(INVALID_PORT_ID)
    {}

    Impl(uint32_t _addr, uint16_t _port) : m_addr(_addr), m_port(_port)
    {}

    std::pair<uint32_t, uint16_t> getValueImpl() const
    {
        return {m_addr, m_port};
    }

private:
    uint32_t m_addr;    // in_addr_t
    uint16_t m_port;    // in_port_t
};

AddrID::AddrID() : impl(std::make_shared<Impl>())
{}

AddrID::AddrID(uint32_t _addr, uint16_t _port) : impl(std::make_shared<Impl>(_addr, _port))
{}

AddrID::~AddrID()
{}

std::pair<uint32_t, uint16_t> AddrID::getValue() const
{
    return impl ? impl->getValueImpl() : std::pair<uint32_t, uint16_t>{INVALID_ADDR_ID, INVALID_PORT_ID};
}