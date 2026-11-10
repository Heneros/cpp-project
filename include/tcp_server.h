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
#include <optional>
#include <expected>

#include "server_observer.h"
#include "client.h"
#include "file_descriptor.h"
#include "pipe_ret_t.h"

class TcpServer
{
private:
    FileDescriptor _sockfd;
    struct sockaddr_storage _serverAddress;
    struct sockaddr_storage _clientAddress;

    socklen_t _serverAddressLength;
    socklen_t _clientAddressLength;

    fd_set _fds;

    // std::vector<Client *> _clients;
    // std::vector<std::unique_ptr<Client>> _clients;
    std::vector<std::shared_ptr<Client>> _clients;
    std::vector<server_observer_t> _subscribers;

    std::mutex _subscribersMtx;
    std::mutex _clientsMtx;

    std::unique_ptr<std::jthread> _clientsRemoverThread;

    std::atomic<bool> _stopRemoveClientsTask;
    void publishClientMsg(const Client &client, const char *msg, size_t msgSize);
    void publishClientDisconnected(const std::string &, const std::string &);
    pipe_ret_t waitForClient(uint32_t timeout);
    void clientEventHandler(const Client &, ClientEvent, const std::string &msg);

    void removeDeadClients();
    void terminateDeadClientsRemover();

    std::expected<void, std::string> sendToClient(const Client &client, const char *msg, size_t size);
    std::optional<std::shared_ptr<Client>> findClientByIp(const std::string &clientIP);

public:
    TcpServer();
    ~TcpServer();

    std::expected<void, std::string> start(int port, int maxNumOfClients = 5, bool removeDeadClientsAutomatically = true);

    void initializeSocket();
    void bindAddress(int port);
    void listenToClients(int maxNumOfClients);
    std::string acceptClient(uint timeout);

    void subscribe(const server_observer_t &observer);

    std::expected<void, std::string> sendToAllClients(const char *msg, size_t size);
    std::expected<void, std::string> sendToClient(const std::string &clientIP, const char *msg, size_t size);

    pipe_ret_t deleteClient(const std::string &clientIp);

    pipe_ret_t close();
    void printClients();
};