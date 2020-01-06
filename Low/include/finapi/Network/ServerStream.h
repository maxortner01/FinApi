/**
 * @file ServerStream.h
 * 
 * @brief Stream object that manages a file-directed connection with the server.
 * 
 * @author  Max Ortner
 * @date    2012-01-02
 * @version 0.1
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "CClient.h"

namespace finapi
{
namespace Cloud
{
    class ServerStream : network::object, public FinStream
    {
        const char*  fname;
        unsigned int it;
        unsigned int filesize;

    public:
        ServerStream(const char* filename, const char* address);
        ServerStream(const char* filename, Cloud::Address address);

        ReadStatus read(char* dest, uint size) override;

        void seek(c_uint bytes);
    };
}
}
