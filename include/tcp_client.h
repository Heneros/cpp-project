#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <errno.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <optional>

#include <cstring>
#include <stdexcept>
#include <memory>

#include "client_observer.h"
#include "pipe_ret_t.h"
#include "file_descriptor.h"

class TcpClient
{
private:
    FileDescriptor _sockfd;
    std::atomic<bool> _isConnected;
    std::atomic<bool> _isClosed;

    struct sockaddr_storage _server;
    socklen_t _addressLength;

    std::vector<client_observer_t> _subscibers;
    std::jthread *_receiveTask = nullptr;
    std::mutex _subscribersMtx;

    // std::shared_ptr<int> actions;
    std::atomic<size_t> _actionsCount{0};

    void initializeSocket();
    void startReceivingMessages();
    void setAddress(const std::string &address, int port);
    void publishServerMsg(std::shared_ptr<std::vector<char>> msg);
    void publishServerDisconnected(const pipe_ret_t &ret);
    void receiveTask();
    void terminateReceiveThread();

public:
    TcpClient();

    ~TcpClient();

    pipe_ret_t connectTo(const std::string &address, int port);
    pipe_ret_t sendMsg(const char *msg, size_t size);

    void subscribe(const client_observer_t &observer);
    bool isConnected() const { return _isConnected; }

    size_t getActionsCount() const
    {
        return _actionsCount.load();
    }

    pipe_ret_t close();
};