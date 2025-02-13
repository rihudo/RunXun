#include "net_tool.hpp"
#include <iostream>
#include "protocol_handler.hpp"
#include "net_tool.hpp"
#include <unistd.h>

using namespace std;

#define PROTOCOL_DEBUG 0
#define SEND_RECV_DEBUG 1

int main()
{
    /* 测试协议处理与生成 */
    #if PROTOCOL_DEBUG
    ProtocolHandler ph;
    Message msg;
    msg.msg_type = MSG_TYPE::hello;
    msg.info = "lhoodxwll";
    std::string generate_str(ph.generateSendMessage(msg));
    Message handler_msg = ph.getMessageFromBuffer(generate_str.c_str(), generate_str.size());
    cout << "MSG::type " << handler_msg.msg_type << endl;
    cout << "MSG::info " << handler_msg.info << endl;
    #endif

    /* 测试收发消息 */
    #if SEND_RECV_DEBUG
    pid_t pid = fork();
    // 子进程接收hello消息, 并回复
    if (0 == pid)
    {
        NetTool nt("0.0.0.0", 8895, {});
        Message recv_hello_msg;
        auto recv_result = nt.recv(recv_hello_msg);
        cout << "recv_result " << recv_result << endl;
        cout << "hello::type " << recv_hello_msg.msg_type << endl;
        cout << "hello::info " << recv_hello_msg.info << endl;

        Message hello_reply_msg;
        hello_reply_msg.msg_type = MSG_TYPE::hello_reply;
        hello_reply_msg.info = "Nice to meet you";
        hello_reply_msg.addr_id = recv_hello_msg.addr_id;
        nt.send(hello_reply_msg);
    }
    // 父进程首先发送hello消息, 随后接收回复
    else
    {
        sleep(1);
        NetTool nt("0.0.0.0", 8894, {8895, 8894});
        Message hello_msg;
        hello_msg.msg_type = MSG_TYPE::hello;
        hello_msg.info = "lhood";
        nt.broadcast(hello_msg);
        Message recv_msg;
        while (-2 == nt.recv(recv_msg))
        {
            cout << "Recvived local broadcast msg" << endl;
        }
        cout << "recv_msg::type " << recv_msg.msg_type << endl;
        cout << "recv_msg::info " << recv_msg.info << endl;
    }
    #endif

    return 0;
}