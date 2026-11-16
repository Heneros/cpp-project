
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <atomic>

#include <sys/epoll.h>
#include "pollable.h"

class Epoll
{
public:
    static constexpr int kMaxEvents = 20;
    static std::shared_ptr<Epoll> GetInstance();
    Epoll(int maxevents);

private:
    int epollfd_;
    int efd_;
    int maxevents_;
    std::atomic<bool> loop_flag_;

    struct epoll_event buildEpollEvents(const std::shared_ptr<Pollable> &poller_ptr, bool want_out = true,
                                        bool want_in = true);
    void onEventFd();
    void onSocketAccept(int listen_sock);
    void onSocketClose(int sock_fd);
    void onSocketRead(int sock_fd);
    void onSocketWrite(int sock_fd);

    std::unordered_map<int, std::shared_ptr<Pollable>> fd_pollers_;
    std::recursive_mutex mutex_;
};