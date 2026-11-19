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

// std::shared_ptr<Epoll> Epoll::GetInstance()
// {
//     static std::shared_ptr<Epoll> single = std::make_shared<Epoll>();
//     return single;
// }

// Epoll::Epoll(int maxEvents) : maxevents_(maxEvents), loop_flag_(false)
// {
//     epollfd_ = ::epoll_create1(0);
//     if (epollfd_ == -1)
//     {
//         std::stringstream errinfo;
//         errinfo << "epoll create1" << strerror(errno);
//         throw std::runtime_error(errinfo.str());
//     }
//     efd_ = eventfd(0, EFD_NONBLOCK);
// }
