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
    class ServerStream : network::object, public StreamCheck
    {
        bool        _ok;
        const char* fname;

    public:
        ServerStream(const char* filename, Cloud::Address address);
        ~ServerStream();

        ReadStatus read(char* dest, c_uint size) const;
    };
}
}
