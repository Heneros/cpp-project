#pragma once
#include <string>
#include <sys/socket.h>

#include "pollable.h"
#include "stream.h"

class Connection : public Stream
{

public:
    Connection(Stream::Type, int listen_fd);
    Connection(int listen_fd) : Connection(Stream::Type::Unframed, listen_fd) {}
    ~Connection();

    Connection(const Connection &) = delete;
    Connection(Connection &&) = delete;

    int Sock() { return sock_fd_; };
    void Enqueue(std::vector<Byte> data);

protected:
    void OnWrite(int sock_fd) override;

private:
    void SetReadReady();
    void ClearReadReady();
    void SetWriteReady();
    void ClearWriteReady();

    int sock_fd_;
};

class Listener : public Acceptable
{
    Listener(int domain, int max_connections);
    Listener(int max_connections) : Listener(AF_INET, max_connections) {};
    ~Listener();

    Listener(const Listener &) = delete;
    Listener(Listener &&) = delete;

    int Sock() { return listen_fd_; };
    void Listen(const std::string &ipaddr, int port);
    void Listen(const std::string &unix_path);

private:
    int domain_;
    int listen_fd_;
    int backlog_;
};