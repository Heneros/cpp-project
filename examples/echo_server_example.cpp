#include <iostream>

#include "../include/listener.h"
#include "../include/epoll.h"

class EchoConnection : public Connection
{

public:
    EchoConnection(int listen_fd) : Connection(listen_fd) {}

    void OnRecv(std::vector<Byte> data) override
    {
        std::string str(data.begin(), data.end());
        std::cout << "Receive " << data.size() << " bytes: ";
        std::cout << str << std::endl;

        std::cout << "Echo back..." << std::endl
                  << std::endl;
        Enqueue(data);
    }
};

class EchoListener : public Listener
{
public:
    static const int kMaxConnections = 20;
    EchoListener() : Listener(kMaxConnections) {};

    void OnAccept(int listen_sock) override
    {
        auto connection_ptr = std::make_shared<EchoConnection>(listen_sock);
        std::cout << "Accept " << connection_ptr->Sock() << " from " << listen_sock << std::endl;
        Epoll::GetInstance()->Join(connection_ptr->Sock(), connection_ptr);
    }
};

int main([[maybe_unused]] int argc,     
         [[maybe_unused]] char *argv[], 
         [[maybe_unused]] char *envp[]) 
{
    auto p_listener = std::make_shared<EchoListener>();

    Epoll::GetInstance()->Join(p_listener->Sock(), p_listener);

    p_listener->Listen("127.0.0.1", 8000);

    Epoll::GetInstance()->Wait();

    return 0;
}