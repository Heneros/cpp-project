
#pragma once

#include <mutex>
#include <vector>

#include "pollable.h"
#include "io_buffer.h"

class Stream : public ReadWritable, public Closable
{
public:
    enum class Type
    {
        Unframed,
        Framed,
    };

    Stream() : Stream(Type::Unframed) {};
    Stream(Type type) : type_(type) {};

    void Enqueue(std::vector<Byte> data);

protected:
    virtual void OnRecv(std::vector<Byte> data) = 0;
    void SetBytesNeeded(std::size_t bytes_needed);

    int PktsEnqueued(void);

    void OnClose(int sock_fd) override { (void)sock_fd; };
    void OnWrite(int sock_fd) override;
    void OnRead(int sock_fd) override;

private:
    void recvUnframed(int sock_fd);
    void recvFramed(int sock_fd);

    Type type_;
    IOBuffer io_buffer_;
    std::mutex mutex_;
};