/**
 * @file CClient.h
 * 
 * @brief Financial API specific network operations.
 * 
 * @author  Max Ortner
 * @date    2019-12-24
 * @version 0.1
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "Network.h"

#ifdef _FIN_DEBUG

#   include <iostream>
#   include <chrono>

#   define time_p std::chrono::_V2::system_clock::time_point
#   define time_point(name) time_p name = std::chrono::high_resolution_clock::now();
#   define logtime_msg(msg, time, post) std::cout << msg << \
    std::chrono::duration_cast<time>(stop - start).count() << post << "\n"

#define logmsg_ms(msg) logtime_msg(msg, std::chrono::milliseconds, " ms")
#define logmsg_micros(msg) logtime_msg(msg, std::chrono::microseconds, " micros")

#else

//  Define the macros as empty so they can be used irreplaceably
#   define time_point(...)
#   define logmsg_micros(...)
#   define logmsg_ms(...)

#endif

#define ADDR_FROM_ENUM(enum) finapi::Cloud::_ADDR::addresses[enum]

namespace finapi
{
namespace Cloud
{
namespace _ADDR
{
    static const char* addresses[] = {
        "127.0.0.1",
        "192.168.1.142",
        "99.83.27.95"
    };
}

    class File;

    enum Address
    {
        LocalHost,
        LocalServer,
        Server
    };

    enum Status
    {
        OK,
        DNE,
        EMPTY,
        SOCKET_FAIL,
        CONNECT_FAIL,
        LOGIN_FAIL
    };

    class StreamCheck
    {
        bool _ok;

    protected:
        void set_ok(bool value) { _ok = value; }

    public:
        StreamCheck() : _ok(false)
        {   }

        const bool ok() const { return _ok; }
        explicit operator bool() const { return ok(); }
    };

    /**
     * @brief Make a request to the server with a given command.
     * 
     * @param command   Command to send to the server
     * @param socket    Socket of the server
     * @param buffer    Location of the buffer to populate
     * @param size      Bytesize of the given buffer
     */
    void make_request(const char* command, const int socket, char* buffer, int size = -1);

    /**
     * @brief Make a request with a given socket.
     * 
     * @param command       Command to send to the server
     * @param socket        Socket connection to the server
     * @return std::string  Response given by the server
     */
    std::string make_request(const char* command, const int socket);

    /**
     * @brief Make a request with a given address.
     * 
     * @param command       Command to send to the server
     * @param address       Address of the server
     * @return std::string  Response given by the server
     */
    std::string make_request(const char* command, const char* address);

    /**
     * @brief Check whether or not a file exists
     * 
     * @param filename  Name of the file to check
     * @param address   Address of the server
     * @return bool     Whether or not the file exists
     */
    bool file_exists(const char* filename, const char* address);

    /**
     * @brief Create a socket solely for the purpose of pulling a file from the server
     * 
     * @param filename  Name of the file to pull
     * @param i         Index of the chunk within the file to pull
     * @param filesize  Size of the file
     * @param buffer    Location of the buffer to populate       
     * @param address   IP Address of the server
     */
    void request_file(const char* filename, const int i, const int filesize, char* buffer, const char* address);
    
    void get_file(const char* filename, const char* address, File& file);

    void get_file(const char* filename, Address address, File& file);
}
}