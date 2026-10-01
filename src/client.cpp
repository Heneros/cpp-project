#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <iostream>

// #include <thread>

#include "../include/client.h"
#include "../include/common.h"

Client::Client(int fileDescriptor)
{
    _sockfd.set(fileDescriptor);
    setConnected(false);
}

bool Client::operator==(const Client &other) const
{
    if ((this->_sockfd.get() == other._sockfd.get()) &&
        (this->_ip == other._ip))
    {
        return true;
    }
    return false;
}

void Client::startListen()
{
    setConnected(true);
    _receiveThread = new std::thread(&Client::receiveTask, this);
}

void Client::send(const char *msg, size_t msgSize) const
{
    const size_t numBytesSent = ::send(_sockfd.get(), (char *)msg, msgSize, 0);
}