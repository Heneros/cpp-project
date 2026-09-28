#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

#include "client_event.h"
#include "file_descriptor.h"

class Client
{
    using client_event_handler_t = std::function<void(const Client &, ClientEvent, const std::string &)>;

private:
    FileDescriptor _sockfd;
    std::string _ip = "";
    std::atomic<bool> _isConnected;
    std::thread *_receivedThread = nullptr;
    client_event_handler_t _eventHandlerCallback;

    void setConnected(bool flag) { _isConnected = flag; }
    void receiveTask();
    void terminateReceiveThread();

public:
    Client(int);

    bool operator==(const Client &other) const;

    std::string getIp() const { return _ip; }

    void startListen();
};