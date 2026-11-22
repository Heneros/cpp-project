#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <memory>
#include <sys/epoll.h>

#include <sys/socket.h>
#include <unistd.h>

#include <spdlog/spdlog.h>
#include <sys/eventfd.h>

#include "../include/epoll.h"

std::shared_ptr<Epoll> Epoll::GetInstance()
{
    static std::shared_ptr<Epoll> single = std::make_shared<Epoll>();
    return single;
}

Epoll::Epoll(int maxEvents) : maxevents_(maxEvents), loop_flag_(false)
{
    epollfd_ = ::epoll_create1(0);
    if (epollfd_ == -1)
    {
        std::stringstream errinfo;
        errinfo << "epoll create1" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }
    efd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    if (efd_ == -1)
    {
        std::stringstream errinfo;
        errinfo << "Failed to create evenfd" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }

    struct epoll_event ev = {};
    ev.events = EPOLLIN;
    ev.data.fd = efd_;

    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, efd_, &ev) == -1)
    {
        std::stringstream errinfo;
        errinfo << "Failed to add eventfd to epoll:" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }
    spdlog::debug("Debug: {}", epollfd_);
}

Epoll::~Epoll()
{
    ::close(efd_);
    ::close(epollfd_);
    spdlog::debug("~Epoll:{}", epollfd_);
}

void Epoll::Join(int sock_fd, std::shared_ptr<Pollable> poller_ptr)
{

    std::lock_guard<std::recursive_mutex> lock(mutex_);

    fd_pollers_.insert({sock_fd, poller_ptr});
    struct epoll_event ev = buildEpollEvents(poller_ptr);
    ev.data.fd = sock_fd;

    if (::epoll_ctl(epollfd_, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
    {
        std::ostringstream errinfo;
        errinfo << "epoll_ctl: EPOLL_CTL_ADD(Join):" << strerror(errno);
        throw std::runtime_error(errinfo.str());
    }
    spdlog::debug("Epoll Join:{} with events:0x{:X}", sock_fd, static_cast<unsigned int>(ev.events));
}
