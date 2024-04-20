#pragma once
#include <cstdint>
#include <memory>

#define INVALID_ADDR_ID 0
#define INVALID_PORT_ID 0

class AddrID
{
public:
    AddrID();
    ~AddrID();

private:
    AddrID(uint32_t _addr, uint16_t _port);
    // 返回0表示失败
    std::pair<uint32_t, uint16_t> getValue() const;

private:
    class Impl;
    std::shared_ptr<Impl> impl;

    friend class NetTool;
    friend class AddrIDHash;
    friend bool operator==(const AddrID&, const AddrID&);
};