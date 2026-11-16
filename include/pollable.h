#pragma once

class Epoll;

class Pollable
{
protected:
    Epoll *epoll_ = nullptr;

public:
    virtual ~Pollable() = default;

    void setEpoll(Epoll *epoll) { epoll_ = epoll; }

    Epoll *epoll() const { return epoll_; }
};

class Acceptable : virtual public Pollable
{
protected:
    virtual void onAccept(int listen_sock) = 0;
    virtual void onAcceptError(int listen_sock) { (void)listen_sock; }
    friend class Epoll;
};

class ReadWritable : virtual public Pollable
{
protected:
    virtual void onRead(int sock_fd) = 0;
    virtual void OnWrite(int sock_fd) = 0;

    virtual void OnDisconnect(int sock_fd) { (void)sock_fd; }
    friend class Epoll;
};

class Closable : virtual public Pollable
{
protected:
    virtual void onClose(int sock_fd) = 0;
    friend class Epoll;
};