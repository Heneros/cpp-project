
#include <functional>
#include <thread>
#include <algorithm>

#include "../include/tcp_server.h"

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

void TcpServer::printClients()
{
    std::lock_guard<std::mutex> lock(_clientsMtx);
    if (_clients.empty())
    {
        std::cout << "no connected clients\n";
    }

    for (const Client *client : _clients)
    {
        client->print();
    }
}

void TcpServer::bindAddress(int port)
{
    memset(&_serverAddress, 0, sizeof(_serverAddress));

    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    _serverAddress.sin_port = htons(port);

    const int bindResult = bind(_sockfd.get(), (struct sockaddr *)&_serverAddress, sizeof(_serverAddress));
    const bool bindFailed = (bindResult == -1);
    if (bindFailed)
    {
        throw std::runtime_error(strerror(errno));
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
        _clientsRemoverThread = new std::thread(&TcpServer::removeDeadClients, this);
    }
    try
    {
        initializeSocket();
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
    std::vector<Client *>::const_iterator clientToRemove;

    while (!_stopRemoveClientsTask)
    {
        std::lock_guard<std::mutex> lock(_clientsMtx);
        do
        {
            clientToRemove = std::find_if(_clients.begin(), _clients.end(), [](Client *client)
                                          { return !client->isConnected(); });

            if (clientToRemove != _clients.end())
            {
                (*clientToRemove)->close();
                delete *clientToRemove;
                _clients.erase(clientToRemove);
            }
        } while (clientToRemove != _clients.end());
    }
    sleep(2);
}

pipe_ret_t TcpServer::close()
{
    terminateDeadClientsRemover();
    {
        std::lock_guard<std::mutex> lock(_clientsMtx);
        for (Client *client : _clients)
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

void TcpServer::terminateDeadClientsRemover()
{
}

void TcpServer::initializeSocket()
{
    _sockfd.set(socket(AF_INET, SOCK_STREAM, 0));
    const bool socketFailed = (_sockfd.get() == -1);
    if (socketFailed)
    {
        throw std::runtime_error(strerror(errno));
    }
    const int option = 1;
    setsockopt(_sockfd.get(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}
