#pragma once

#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>


#include <thread>
#include <functional>
#include <cstring>
#include <errno.h>
#include <iostream>

#include "file_descriptor.h"

class TcpServer
{
private:
    FileDescriptor _sockfd;
    struct sockaddr_in _serverAddress;
    struct sockaddr_in _clientAddress;
};