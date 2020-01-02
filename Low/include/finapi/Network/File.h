#pragma once

#include "CClient.h"

namespace finapi
{
namespace Cloud
{
    struct File : public StreamCheck
    {
        Status status;
        c_uint filesize;
        char*  buffer;

        File(Status s = EMPTY);

        File(c_uint size);

        void read(void* ptr, c_uint size);

        ~File();
    
    private:
        unsigned int iterator;
    };
}
}