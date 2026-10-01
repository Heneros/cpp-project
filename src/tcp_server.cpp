
#include <functional>
#include <thread>
#include <algorithm>

#include "../include/tcp_server.h"

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
}
pipe_ret_t TcpServer::start(int port, int maxNumOfClients, bool removeDeadClientsAutomatically)
{
    // if(removeDeadClientsAutomatically){

    // }
    // initializeSocket();
    return pipe_ret_t::success();
}