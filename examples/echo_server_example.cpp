#include <iostream>

#include "epoll.h"
#include "../include/listener.h"

class EchoConnection : public Connection
{

public:
    EchoConnection(int listen_fd) : Connection(listen_fd) {}

    void OnRecv(std::vector<Byte> data) override
    {
        std::string str(data.begin(), data.end());
    }
};