
#include <functional>
#include <thread>
#include <algorithm>
#include <netdb.h>

#include "../include/tcp_server.h"
#include "../include/common.h"

TcpServer::TcpServer()
{
    _subscribers.reserve(10);
    _clients.reserve(10);
    _stopRemoveClientsTask = false;
}

TcpServer::~TcpServer()
{
    close();
}

void TcpServer::subscribe(const server_observer_t &observer)
{
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    _subscribers.push_back(observer);
}

void TcpServer::printClients()
{
    std::lock_guard<std::mutex> lock(_clientsMtx);
    if (_clients.empty())
    {
        std::cout << "no connected clients\n";
    }

    for (const auto &client : _clients)
    {
        client->print();
    }
}

pipe_ret_t TcpServer::deleteClient(const std::string &clientIp)
{
    try
    {
        std::lock_guard<std::mutex> lock(_clientsMtx);

        auto clientIter = std::find_if(_clients.begin(), _clients.end(), [&clientIp](const auto &client)
                                       { return client->getIp() == clientIp; });

        if (clientIter == _clients.end())
        {
            return pipe_ret_t::failure("Client not found");
        }

        _clients.erase(clientIter);

        return pipe_ret_t::success();
    }
    catch (const std::runtime_error &error)
    {
        return pipe_ret_t::failure(error.what());
    }
}

void TcpServer::bindAddress(int port)
{
    struct addrinfo hints{}, *res = nullptr, *p = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(port);
    int status = getaddrinfo(nullptr, port_str.c_str(), &hints, &res);

    if (status != 0)
    {
        throw std::runtime_error(std::string("getaddrinfo failed: ") + gai_strerror(status));
    }

    int sockfd = -1;
    for (p = res; p != nullptr; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        int option = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0)
        {
            _sockfd.set(sockfd);
            std::memcpy(&_serverAddress, p->ai_addr, p->ai_addrlen);
            _serverAddressLength = p->ai_addrlen;
            break;
        }

        ::close(sockfd);
        sockfd = -1;
    }
    freeaddrinfo(res);

    if (sockfd == -1)
    {
        throw std::runtime_error("bind failed on all available addresses");
    }
}

void TcpServer::listenToClients(int maxNumOfClients)
{

    const int clientsQueueSize = maxNumOfClients;
    const bool listenFailed = (listen(_sockfd.get(), clientsQueueSize) == -1);
    if (listenFailed)
    {
        throw std::runtime_error(strerror(errno));
    }
}

pipe_ret_t TcpServer::start(int port, int maxNumOfClients, bool removeDeadClientsAutomatically)
{
    if (removeDeadClientsAutomatically)
    {
        _clientsRemoverThread = std::make_unique<std::jthread>([this]
                                                               { removeDeadClients(); });
    }
    try
    {
        // initializeSocket();
        bindAddress(port);
        listenToClients(maxNumOfClients);
    }
    catch (const std::runtime_error &error)
    {
        return pipe_ret_t::failure(error.what());
    }
    return pipe_ret_t::success();
}

void TcpServer::removeDeadClients()
{
    // std::vector<Client *>::const_iterator clientToRemove;

    while (!_stopRemoveClientsTask.load())
    {
        {
            std::lock_guard<std::mutex> lock(_clientsMtx);
            auto clientToRemove = _clients.begin();

            while (clientToRemove != _clients.end())
            {
                /// Client *client = clientToRemove->get();
                if (!(*clientToRemove)->isConnected())
                {
                    clientToRemove = _clients.erase(clientToRemove);
                }
                else
                {
                    ++clientToRemove;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

pipe_ret_t TcpServer::close()
{
    terminateDeadClientsRemover();
    {
        std::lock_guard<std::mutex> lock(_clientsMtx);
        for (const auto &client : _clients)
        {
            try
            {
                client->close();
            }
            catch (const std::runtime_error &error)
            {
                return pipe_ret_t::failure(error.what());
            }
        }
        _clients.clear();
    }
    {
        const int closeServerResult = ::close(_sockfd.get());
        const bool closeServerFailed = (closeServerResult == -1);
        if (closeServerFailed)
        {
            return pipe_ret_t::failure(strerror(errno));
        }
    }
    return pipe_ret_t::success();
}

void TcpServer::initializeSocket()
{
    //_sockfd.set(socket(AF_INET, SOCK_STREAM, 0));
    _sockfd.set(socket(AF_INET6, SOCK_STREAM, 0));
    const bool socketFailed = (_sockfd.get() == -1);
    if (socketFailed)
    {
        throw std::runtime_error(strerror(errno));
    }
    const int option = 1;
    setsockopt(_sockfd.get(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

void TcpServer::terminateDeadClientsRemover()
{
    if (_clientsRemoverThread)
    {
        _stopRemoveClientsTask = true;
        _clientsRemoverThread->join();
        // delete _clientsRemoverThread;
        _clientsRemoverThread = nullptr;
    }
}

void TcpServer::publishClientMsg(const Client &client, const char *msg, size_t msgSize)
{
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    for (const server_observer_t &subscriber : _subscribers)
    {
        if (subscriber.wantedIP == client.getIp() || subscriber.wantedIP.empty())
        {
            if (subscriber.incomingPacketHandler)
            {
                subscriber.incomingPacketHandler(client.getIp(), msg, msgSize);
            }
        }
    }
}

void TcpServer::clientEventHandler(const Client &client, ClientEvent event, const std::string &msg)
{
    switch (event)
    {
    case ClientEvent::DISCONNECTED:
    {
        publishClientDisconnected(client.getIp(), msg);
        break;
    }
    case ClientEvent::INCOMING_MSG:
    {
        publishClientMsg(client, msg.c_str(), msg.size());
        break;
    }
    }
}
void TcpServer::publishClientDisconnected(const std::string &clientIP, const std::string &clientMsg)
{
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    for (const server_observer_t &subscriber : _subscribers)
    {
        if (subscriber.wantedIP == clientIP)
        {
            if (subscriber.disconnectionHandler)
            {
                subscriber.disconnectionHandler(clientIP, clientMsg);
            }
        }
    }
}

std::string TcpServer::acceptClient(uint timeout)
{
    const pipe_ret_t waitingForClient = waitForClient(timeout);

    if (!waitingForClient.isSuccessful())
    {
        throw std::runtime_error(waitingForClient.message());
    }
    socklen_t socketSize = sizeof(_clientAddress);

    const int fileDescriptor = accept(_sockfd.get(), (struct sockaddr *)&_clientAddress, &socketSize);

    if (fileDescriptor == -1)
    {
        if (errno == ECONNABORTED || errno == EAGAIN)
        {
            return "";
        }

        throw std::runtime_error("accept failed: " + std::string(strerror(errno)));
    }
    char ipStr[INET6_ADDRSTRLEN];
    std::string clientIp;

    if (_clientAddress.ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4_addr = (struct sockaddr_in *)&_clientAddress;
        if (inet_ntop(AF_INET, &ipv4_addr->sin_addr, ipStr, sizeof(ipStr)) == nullptr)
        {
            ::close(fileDescriptor);
            throw std::runtime_error("Failed to convert IPv4 address to string");
        }
        clientIp = ipStr;
    }
    else if (_clientAddress.ss_family == AF_INET6)
    {
        struct sockaddr_in6 *ipv6_addr = (struct sockaddr_in6 *)&_clientAddress;
        if (IN6_IS_ADDR_V4MAPPED(&ipv6_addr->sin6_addr))
        {
            struct in_addr ipv4_addr;
            std::memcpy(&ipv4_addr, &ipv6_addr->sin6_addr.s6_addr[12], 4);
            if (inet_ntop(AF_INET, &ipv4_addr, ipStr, sizeof(ipStr)) == nullptr)
            {
                ::close(fileDescriptor);
                throw std::runtime_error("Failed to convert IPv4 address to string");
            }
            clientIp = ipStr;
        }
        else
        {
            if (inet_ntop(AF_INET6, &ipv6_addr->sin6_addr, ipStr, sizeof(ipStr)) == nullptr)
            {
                ::close(fileDescriptor);
                throw std::runtime_error("Failed to convert IPv6 address to string");
            }
            clientIp = ipStr;
        }
    }
    else
    {
        ::close(fileDescriptor);
        throw std::runtime_error("Unknown address family");
    }

    auto newClient = std::make_unique<Client>(fileDescriptor);
    newClient->setIp(clientIp);
    using namespace std::placeholders;
    newClient->setEventsHandler(std::bind(&TcpServer::clientEventHandler, this, _1, _2, _3));
    newClient->startListen();

    {
        std::lock_guard<std::mutex> lock(_clientsMtx);
        _clients.push_back(std::move(newClient));
    }

    return clientIp;
}

pipe_ret_t TcpServer::waitForClient(uint32_t timeout)
{
    if (timeout > 0)
    {
        const fd_wait::Result waitResult = fd_wait::waitFor(_sockfd, timeout);

        if (waitResult == fd_wait::Result::FAILURE)
        {
            return pipe_ret_t::failure(strerror(errno));
        }
        else if (waitResult == fd_wait::Result::TIMEOUT)
        {
            return pipe_ret_t::failure("Timeout waiting for client");
        }
    }
    return pipe_ret_t::success();
}
pipe_ret_t TcpServer::sendToAllClients(const char *msg, size_t size)
{
    // std::lock_guard<std::mutex> lock(_clientsMtx);
    std::vector<std::shared_ptr<Client>> local_clients;

    {
        std::lock_guard<std::mutex> lock(_clientsMtx);
        local_clients = _clients;
    }

    if (local_clients.empty())
    {
        return pipe_ret_t::failure("No clients connected");
    }

    bool atLeastOneSuccess = false;

    for (const auto &client : local_clients)
    {
        pipe_ret_t sendingResult = sendToClient(*client, msg, size);

        if (sendingResult.isSuccessful())
        {
            atLeastOneSuccess = true;
        }
        else
        {
            std::cout << "[ERROR] sendingResult: " << sendingResult.message() << "\n";
        }
    }

    if (!atLeastOneSuccess)
    {
        return pipe_ret_t::failure("Failed to send to any client");
    }

    return pipe_ret_t::success();
}

pipe_ret_t TcpServer::sendToClient(const Client &client, const char *msg, size_t size)
{
    try
    {

        client.send(msg, size);
    }
    catch (const std::runtime_error &error)
    {

        return pipe_ret_t::failure(error.what());
    }
    return pipe_ret_t::success();
}

pipe_ret_t TcpServer::sendToClient(const std::string &clientIP, const char *msg, size_t size)
{
    std::lock_guard<std::mutex> lock(_clientsMtx);

    const auto clientIter = std::find_if(_clients.begin(), _clients.end(),
                                         [&clientIP](const auto &client)
                                         { return client->getIp() == clientIP; });

    if (clientIter == _clients.end())
    {
        return pipe_ret_t::failure("client not found");
    }

    const Client &client = *(*clientIter);
    return sendToClient(client, msg, size);
}
