#pragma once

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <thread>
#include <functional>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <mutex>

#include "server_observer.h"
#include "client.h"
#include "file_descriptor.h"
#include "pipe_ret_t.h"

class TcpServer
{
private:
    FileDescriptor _sockfd;
    struct sockaddr_in _serverAddress;
    struct sockaddr_in _clientAddress;

    fd_set _fds;

    std::vector<Client *> _clients;
    std::vector<server_observer_t> _subscribers;

    std::mutex _subscribersMtx;
    std::mutex _clientsMtx;

public:
    TcpServer();
    ~TcpServer();

    void printClients();

    pipe_ret_t start(int port, int maxNumOfClients = 5, bool removeDeadClientsAutomatically = true);
};