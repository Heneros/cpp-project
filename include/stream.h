
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
};