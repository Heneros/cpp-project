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
    virtual void OnAccept(int listen_sock) = 0;
    virtual void OnAcceptError(int listen_sock) { (void)listen_sock; }
    friend class Epoll;
};

class ReadWritable : virtual public Pollable
{
protected:
    virtual void OnRead(int sock_fd) = 0;
    virtual void OnClose(int sock_fd) = 0;
    virtual void OnWrite(int sock_fd) = 0;

    virtual void OnDisconnect(int sock_fd) { (void)sock_fd; }
    friend class Epoll;
};

class Closable : virtual public Pollable
{
protected:
    virtual void OnClose(int sock_fd) = 0;
    friend class Epoll;
};