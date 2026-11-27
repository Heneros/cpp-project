

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <cstdio>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include <arpa/inet.h>
#include <errno.h>

#include <netinet/in.h>
#include <string.h>

#include <unistd.h>

#include "../include/listener.h"
#include "../include/epoll.h"

Connection::Connection(Stream::Type type, int listen_fd) : Stream(type)
{

    SockAddr addr;
    socklen_t addrlen = sizeof(addr);

    sock_fd_ = ::accept(listen_fd, &addr.addr.sa, &addrlen);
    if (sock_fd_ == -1)
    {
        std::stringstream errinfo;
        errinfo << "accept:" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }

    ::fcntl(sock_fd_, F_SETFL, ::fcntl(sock_fd_, F_GETFL) | O_NONBLOCK);

    ::fcntl(sock_fd_, F_SETFD, FD_CLOEXEC);

    printf("[DEBUG] Connection:%d\n", sock_fd_);
}

Connection::~Connection()
{
    ::close(sock_fd_);
    printf("~Connection:{}%d\n", sock_fd_);
}

void Listener::Listen(const std::string &ipaddr, int port)
{
    int optval = 1;
    socklen_t optlen = sizeof(optval);

    ::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);

    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;

    struct sockaddr *toaddr = nullptr;

    socklen_t addrlen = 0;

    if (AF_INET == domain_)
    {
        toaddr = (struct sockaddr *)&addr4;
        addrlen = sizeof(struct sockaddr_in);

        addr4.sin_family = static_cast<sa_family_t>(domain_);
        addr4.sin_port = ::htons(static_cast<uint16_t>(port));

        if (::inet_pton(domain_, ipaddr.c_str(), &addr4.sin_addr) != 1)
        {
            std::stringstream errinfo;
            errinfo << "inet_pton: " << strerror(errno);
            throw std::runtime_error(errinfo.str());
        }
    }
    else if (AF_INET6 == domain_)
    {
        toaddr = (struct sockaddr *)&addr6;
        addrlen = sizeof(struct sockaddr_in6);

        addr6.sin6_family = static_cast<sa_family_t>(domain_);
        addr6.sin6_port = ::htons(static_cast<uint16_t>(port));

        if (::inet_pton(domain_, ipaddr.c_str(), &addr6.sin6_addr) != 1)
        {
            std::stringstream errinfo;
            errinfo << "inet_pton:" << strerror(errno);
            throw std::runtime_error(errinfo.str());
        }
    }

    if (::bind(listen_fd_, toaddr, addrlen) == -1)
    {
        std::stringstream errinfo;
        errinfo << "bind:" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }

    if (::listen(listen_fd_, backlog_) == -1)
    {
        std::stringstream errinfo;
        errinfo << "listen:" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }
}

void Listener::Listen(const std::string &unix_path)
{
    int optval = 1;
    socklen_t optlen = sizeof(optval);
    ::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);

    struct sockaddr_un addr = {};
    addr.sun_family = static_cast<sa_family_t>(domain_);

    std::strncpy(addr.sun_path, unix_path.c_str(), sizeof(addr.sun_path) - 1);
    addr.sun_family = static_cast<sa_family_t>(domain_);
    unlink(unix_path.c_str());

    if (::bind(listen_fd_, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::stringstream errinfo;
        errinfo << "bind:" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }

    if (::listen(listen_fd_, backlog_) == -1)
    {
        std::stringstream errinfo;
        errinfo << "listen:" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }
}

void Connection::Enqueue(std::vector<Byte> data)
{
    Stream::Enqueue(std::move(data));
    if (PktsEnqueued() > 0)
        SetWriteReady();
}

void Connection::OnWrite(int sock_fd)
{
    Stream::OnWrite(sock_fd);
    if (PktsEnqueued() == 0)
        ClearWriteReady();
}

void Connection::SetWriteReady()
{
    if (auto e = epoll())
        e->DataOutReady(Sock());
}

void Connection::ClearWriteReady()
{
    if (auto e = epoll())
        e->DataOutClear(Sock());
}