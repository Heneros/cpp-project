
#include "../include/tcp_client.h"
#include "../include/common.h"

TcpClient::TcpClient()
{
    _isConnected = false;
    _isClosed = true;
}

TcpClient::~TcpClient()
{
    close();
}

pipe_ret_t TcpClient::connectTo(const std::string &address, int port)
{
    try
    {
        initializeSocket();
        setAddress(address, port);
    }
    catch (const std::runtime_error &error)
    {
        return pipe_ret_t::failure(error.what());
    }

    const int connectResult = connect(_sockfd.get(), (struct sockaddr *)&_server, sizeof(_server));

    const bool connectionFailed = (connectResult == -1);
    if (connectionFailed)
    {
        return pipe_ret_t::failure(strerror(errno));
    }

    startReceivingMessages();
    _isConnected = true;
    _isClosed = false;

    return pipe_ret_t::success();
}

void TcpClient::startReceivingMessages()
{
    _receiveTask = new std::thread(&TcpClient::receiveTask, this);
}

void TcpClient::setAddress(const std::string &address, int port)
{
    const int inetSuccess = inet_aton(address.c_str(), &_server.sin_addr);
    if (!inetSuccess)
    {
        struct hostent *host;
        struct in_addr **addrList;
        if ((host = gethostbyname(address.c_str())) == nullptr)
        {
            throw std::runtime_error("Failed to resolve hostname");
        }
        addrList = (struct in_addr **)host->h_addr_list;
        _server.sin_addr = *addrList[0];
    }
    _server.sin_family = AF_INET;
    _server.sin_port = htons(port);
}
/*
 * Receive server packets, and notify user
 */
void TcpClient::receiveTask()
{
    while (_isConnected)
    {
        const fd_wait::Result waitResult = fd_wait::waitFor(_sockfd);
        if (waitResult == fd_wait::Result::FAILURE)
        {
            throw std::runtime_error(strerror(errno));
        }
        else if (waitResult == fd_wait::Result::TIMEOUT)
        {
            continue;
        }

        char msg[MAX_PACKET_SIZE];
        const size_t numOfBytesReceived = recv(_sockfd.get(), msg, MAX_PACKET_SIZE, 0);

        if (numOfBytesReceived < 1)
        {
            std::string errorMsg;
            if (numOfBytesReceived == 0)
            {
                errorMsg = "Server closed connection";
            }
            else
            {
                errorMsg = strerror(errno);
            }
            _isConnected = false;
            publishServerDisconnected(pipe_ret_t::failure(errorMsg));
            return;
        }
        else
        {
            publishServerMsg(msg, numOfBytesReceived);
        }
    }
}
void TcpClient::publishServerMsg(const char *msg, size_t msgSize)
{
}
void TcpClient::publishServerDisconnected(const pipe_ret_t &ret)

{
    std::lock_guard<std::mutex> lock(_subscribersMtx);
}

void TcpClient::subscribe(const client_observer_t &observer)
{
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    _subscibers.push_back(observer);
}