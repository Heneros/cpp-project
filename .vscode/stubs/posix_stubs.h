#pragma once


#include <cstddef> // size_t
#include <cstdint> // int32_t, uint32_t

// --- <fcntl.h> ---
extern "C"
{


#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif
#ifndef F_SETFD
#define F_SETFD 2
#endif
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 00004000
#endif
#ifndef O_RDWR
#define O_RDWR 2
#endif
#ifndef O_RDONLY
#define O_RDONLY 0
#endif
#ifndef O_WRONLY
#define O_WRONLY 1
#endif
#ifndef O_CREAT
#define O_CREAT 00000100
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 02000000
#endif

    int fcntl(int fd, int cmd, ...);
    int open(const char *pathname, int flags, ...);

} // extern "C"

// --- <sys/socket.h> ---
#ifndef AF_INET
#define AF_INET 2
#endif
#ifndef AF_UNIX
#define AF_UNIX 1
#endif
#ifndef SOCK_STREAM
#define SOCK_STREAM 1
#endif
#ifndef SOCK_DGRAM
#define SOCK_DGRAM 2
#endif
#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK 0x80000
#endif
#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC 0x80000
#endif
#ifndef SOL_SOCKET
#define SOL_SOCKET 1
#endif
#ifndef SO_REUSEADDR
#define SO_REUSEADDR 2
#endif
#ifndef SO_KEEPALIVE
#define SO_KEEPALIVE 9
#endif
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x4000
#endif
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0x40
#endif

extern "C"
{

    typedef unsigned int socklen_t;

    struct sockaddr
    {
        unsigned short sa_family;
        char sa_data[14];
    };

    int socket(int domain, int type, int protocol);
    int bind(int fd, const struct sockaddr *addr, socklen_t addrlen);
    int listen(int fd, int backlog);
    int accept(int fd, struct sockaddr *addr, socklen_t *addrlen);
    int connect(int fd, const struct sockaddr *addr, socklen_t addrlen);
    ssize_t send(int fd, const void *buf, size_t len, int flags);
    ssize_t recv(int fd, void *buf, size_t len, int flags);
    ssize_t sendto(int fd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen);
    ssize_t recvfrom(int fd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, socklen_t *addrlen);
    int shutdown(int fd, int how);
    int setsockopt(int fd, int level, int optname,
                   const void *optval, socklen_t optlen);
    int getsockopt(int fd, int level, int optname,
                   void *optval, socklen_t *optlen);

} // extern "C"

// --- <netinet/in.h> ---
#ifndef INADDR_ANY
#define INADDR_ANY 0u
#endif
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001u
#endif
#ifndef INADDR_BROADCAST
#define INADDR_BROADCAST 0xffffffffu
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP 6
#endif
#ifndef IPPROTO_UDP
#define IPPROTO_UDP 17
#endif
#ifndef TCP_NODELAY
#define TCP_NODELAY 1
#endif

#include <cstdint>

extern "C"
{

    struct in_addr
    {
        uint32_t s_addr;
    };

    struct sockaddr_in
    {
        uint16_t sin_family;
        uint16_t sin_port;
        struct in_addr sin_addr;
        char sin_zero[8];
    };

    uint32_t htonl(uint32_t hostlong);
    uint16_t htons(uint16_t hostshort);
    uint32_t ntohl(uint32_t netlong);
    uint16_t ntohs(uint16_t netshort);

} // extern "C"

// --- <arpa/inet.h> ---
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

extern "C"
{

    int inet_pton(int af, const char *src, void *dst);
    const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

} // extern "C"

// --- <unistd.h> ---
extern "C"
{

    ssize_t read(int fd, void *buf, size_t count);
    ssize_t write(int fd, const void *buf, size_t count);
    int close(int fd);
    int epoll_create1(int flags);
    int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

} // extern "C"

// --- <sys/epoll.h> ---
#ifndef EPOLLIN
#define EPOLLIN 0x001
#endif
#ifndef EPOLLOUT
#define EPOLLOUT 0x004
#endif
#ifndef EPOLLERR
#define EPOLLERR 0x008
#endif
#ifndef EPOLLHUP
#define EPOLLHUP 0x010
#endif
#ifndef EPOLLET
#define EPOLLET (1u << 31)
#endif
#ifndef EPOLLONESHOT
#define EPOLLONESHOT (1u << 30)
#endif
#ifndef EPOLL_CTL_ADD
#define EPOLL_CTL_ADD 1
#endif
#ifndef EPOLL_CTL_DEL
#define EPOLL_CTL_DEL 2
#endif
#ifndef EPOLL_CTL_MOD
#define EPOLL_CTL_MOD 3
#endif
#ifndef EPOLL_CLOEXEC
#define EPOLL_CLOEXEC 02000000
#endif

extern "C"
{

    typedef union epoll_data
    {
        void *ptr;
        int fd;
        uint32_t u32;
        uint64_t u64;
    } epoll_data_t;

    struct epoll_event
    {
        uint32_t events;
        epoll_data_t data;
    } __attribute__((packed));

} // extern "C"

// --- <sys/eventfd.h> ---
#ifndef EFD_NONBLOCK
#define EFD_NONBLOCK 00004000
#endif
#ifndef EFD_CLOEXEC
#define EFD_CLOEXEC 02000000
#endif

extern "C"
{
    int eventfd(unsigned int initval, int flags);
}

// --- <errno.h> ---
#ifndef EAGAIN
#define EAGAIN 11
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif
#ifndef EINTR
#define EINTR 4
#endif
#ifndef EMFILE
#define EMFILE 24
#endif
#ifndef ENOENT
#define ENOENT 2
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED 111
#endif
#ifndef EPIPE
#define EPIPE 32
#endif

// --- <signal.h> ---
#ifndef SIGPIPE
#define SIGPIPE 13
#endif
#ifndef SIGCHLD
#define SIGCHLD 17
#endif
#ifndef SIG_IGN
#define SIG_IGN ((void (*)(int))1)
#endif

extern "C"
{
    void (*signal(int sig, void (*func)(int)))(int);
    int raise(int sig);
}