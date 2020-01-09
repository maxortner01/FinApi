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
        unsigned int fsize;
        Status       _status;

    public:
        ServerStream(const char* filename, const char* address);
        ServerStream(const char* filename, Cloud::Address address);

        ReadStatus read(char* dest, uint size) override;

        c_uint& filesize() const { return fsize;            }
        bool    exists()   const { return (_status != DNE); }
        Status  status()   const { return _status;          }
    };
}
}
