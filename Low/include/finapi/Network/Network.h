/**
 * @file Network.h
 * 
 * @brief Cross-platform foundation for performing basic socket operations.
 * 
 * @author  Max Ortner
 * @date    2019-12-24
 * @version 0.1
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

/*
 * Windows includes and function definitions
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)

/*           INCLUDES          */
#   include <WinSock2.h> // socket methods
#   include <WS2tcpip.h> // For WSA

#   define _FIN_WINDOWS
#   define SOCK_OPT SO_REUSEADDR // Tedious differences in the OS-specific libraries
#   define close_connection(socket) closesocket(socket)


/*     FUNCTION DEFINITIONS     */

/**
 * @brief Function to convert string ip-address into bytes.
 * 
 * @param af   Protocol
 * @param src  IP Address
 * @param dst  The location where the address should go
 * @return int Success value
 */
static int inet_pton(int af, const char *src, void *dst);

namespace finapi
{
namespace network
{
    /**
     * @brief Somewhere in the code, we need to call WSAStartup once.
     */
    static void WSAStart()
    {
        static bool start = false;
        if (!start)
        {
            WSAData data;
            WSAStartup(MAKEWORD(2, 2), &data);
            start = true;
        }
    }
}
}

/*
 * Linux / Unix includes
 */
#else

/*           INCLUDES          */
#   include <sys/socket.h> // socket methods
#   include <netinet/in.h>
#   include <arpa/inet.h>  // inet function
#   include <unistd.h>     // close

#   define SOCK_OPT SO_REUSEADDR | SO_REUSEPORT

#   define close_connection(socket) close(socket)

/*     FUNCTION DEFINITIONS     */
namespace finapi
{
namespace network
{
    static void WSAStart() { return; }
}
}

#endif

// Buffer size to expect from the server
#define _FIN_BUFFER_SIZE 1024*2
#define _FIN_PORT        1420

#include "../Core/Core.h"

namespace finapi
{
namespace network
{
    /**
     * @brief Simple container for holding socket and address information.
     */
    struct object
    {
        int          _socket;
        sockaddr_in* _addr;

        ~object();
    };

    struct mac
    {
        unsigned char address[6];
    };

    /**
     * @brief Get the MAC Address of the network interface.
     * 
     * @return mac MAC Address object
     */
    mac get_mac_address();

    /**
     * @brief Creates a socket and returns the handle.
     * 
     * If the handle is zero then the socket failed to initialize.
     * 
     * @return int Handle of the socket.
     */
    int make_socket();

    /**
     * @brief Sets the basic options of a given socket.
     * 
     * @param sock Integer handle for the socket.
     * @return int Execution state: 0 for failed, 1 for success
     */
    int set_options(const int sock);

    /**
     * @brief Binds a port to a given socket.
     * 
     * @param sock          Socket handle
     * @param port          Port to bind the socket to
     * @return sockaddr_in* Allocated address object containing the information about the socket.
     */
    sockaddr_in* bind_socket(const int sock, const int port);

    /**
     * @brief Sets a given socket to listen for incoming connections
     * 
     * @param sock  Socket handle
     * @param queue Connection backlog count
     * @return int  Success value
     */
    int make_listen(const int sock, const int queue);

    /**
     * @brief Waits for an outside connection and pulls in the incoming socket information.
     * 
     * @param sock      Server socket to listen with
     * @param in        Socket address of client
     * @param addr_len  Size of the socket address
     * @return int      Success value
     */
    int accept_socket(const int sock, sockaddr_in* in, int* addr_len);

    /**
     * @brief Client side function for connecting to a given IP.
     * 
     * @param sock  Client's socket handle
     * @param ip    String of an IP to connect to
     * @param port  Port to connect to
     * @return int  Success value
     */
    int connect_to_ip(const int sock, const char* ip, const int port);

    /**
     * @brief Method for easily connecting to a given IP address.
     * 
     * @param address   String of the IP to connect to
     * @return int      Created socket for connection
     */
    int connect_socket(const char* address);

    /* const char* concatenizer */
    template<typename T>
    static std::string str_concat(T t)
    {
        return std::string(t);
    }

    template<typename T, typename... Args>
    static std::string str_concat(T first, Args... rest)
    {
        return str_concat(first) + str_concat(rest...);
    }

    template<typename... Args>
    static std::string str_concat(Args... rest)
    {
        return str_concat(rest...);
    }
}
}