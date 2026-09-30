#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <iostream>

#include "../include/client.h"
#include "../include/common.h"

Client::Client(int fileDescriptor)
{
    _sockfd.set(fileDescriptor);
    setConnected(false);
}