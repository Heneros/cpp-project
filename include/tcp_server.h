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

    std::thread * _clientsRemoverThread = nullptr;
    std::atomic<bool> _stopRemoveClientsTask;
    void publishClientMsg(const Client &client, const char *msg, size_t msgSize);
    void publishClientDisconnected(const std::string &, const std::string &);
    pipe_ret_t waitForClient(uint32_t timeout);
    void clientEventHandler(const Client &, ClientEvent, const std::string &msg);
  
    void removeDeadClients();
    void terminateDeadClientsRemover();
    static pipe_ret_t sendToClient(const Client &client, const char *msg, size_t size);

public:
    TcpServer();
    ~TcpServer();
    pipe_ret_t connectTo(const std::string &address, int port);
    pipe_ret_t sendMsg(const char *msg, size_t size);
    void printClients();

    pipe_ret_t start(int port, int maxNumOfClients = 5, bool removeDeadClientsAutomatically = true);

    
    void printClients();
};