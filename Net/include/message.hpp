#pragma once
#include "addr_id.hpp"
#include <string>

enum MSG_TYPE
{
    invalid_type,
    hello,
    hello_reply,
    msg,
    msg_reply,
    bye
};

struct Message
{
    MSG_TYPE msg_type;
    AddrID addr_id;
    std::string info;
};