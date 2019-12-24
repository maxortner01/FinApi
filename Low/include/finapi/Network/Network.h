#pragma once

/*
 * Windows includes and function definitions
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || !defined(__CYGWIN__)

/*           INCLUDES          */
#   include <WinSock2.h> // socket methods
#   include <WS2tcpip.h> // For WSA

#   define _FIN_WINDOWS
#   define SOCK_OPT SO_REUSEADDR // Tedious differences in the OS-specific libraries

/*     FUNCTION DEFINITIONS     */

/**
 * @brief Function to convert string ip-address into bytes.
 * 
 * @param af   Protocol
 * @param src  IP Address
 * @param dst  The location where the address should go
 * @return int Success value
 */
static int inet_pton(int af, const char *src, void *dst)
{
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN+1];

    ZeroMemory(&ss, sizeof(ss));
    /* stupid non-const API */
    strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
        switch(af) {
        case AF_INET:
        *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
        return 1;
        case AF_INET6:
        *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
        return 1;
        }
    }
    return 0;
}

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

#   define SOCK_OPT SO_REUSEADDR | SO_REUSEPORT

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

#include <cstdlib>  // malloc, free
#include <string>   // string class
#include <cstring>  // memset

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

        ~object() { std::free(_addr); }
    };

    /**
     * @brief Creates a socket and returns the handle.
     * 
     * If the handle is zero then the socket failed to initialize.
     * 
     * @return int Handle of the socket.
     */
    static int make_socket()
    {
        WSAStart();
        return socket(AF_INET, SOCK_STREAM, 0);
    }

    /**
     * @brief Sets the basic options of a given socket.
     * 
     * @param sock Integer handle for the socket.
     * @return int Execution state: 0 for failed, 1 for success
     */
    static int set_options(const int sock)
    {
        int opt;

        if ( setsockopt(sock, SOL_SOCKET, SOCK_OPT, (char*)&opt, sizeof(int)) )
            return 0;
        return 1;
    }

    /**
     * @brief Binds a port to a given socket.
     * 
     * @param sock          Socket handle
     * @param port          Port to bind the socket to
     * @return sockaddr_in* Allocated address object containing the information about the socket.
     */
    static sockaddr_in* bind_socket(const int sock, const int port)
    {
        sockaddr_in* address = new sockaddr_in;
        int sock_len = sizeof(address);

        address->sin_family = AF_INET;
        address->sin_addr.s_addr = INADDR_ANY;
        address->sin_port = htons(port);

        if ( bind(sock, (sockaddr*)address, sizeof(sockaddr_in)) < 0 )
        {
            std::free(address);
            return 0;
        }

        return address;
    } 

    /**
     * @brief Sets a given socket to listen for incoming connections
     * 
     * @param sock  Socket handle
     * @param queue Connection backlog count
     * @return int  Success value
     */
    static int make_listen(const int sock, const int queue)
    {
        if (listen(sock, queue) < 0)
            return 0;
        return 1;
    }

    /**
     * @brief Waits for an outside connection and pulls in the incoming socket information.
     * 
     * @param sock      Server socket to listen with
     * @param in        Socket address of client
     * @param addr_len  Size of the socket address
     * @return int      Success value
     */
    static int accept_socket(const int sock, sockaddr_in* in, int* addr_len)
    {
        return accept(sock, (sockaddr*)in, (socklen_t*)addr_len);
    }

    /**
     * @brief Client side function for connecting to a given IP.
     * 
     * @param sock  Client's socket handle
     * @param ip    String of an IP to connect to
     * @param port  Port to connect to
     * @return int  Success value
     */
    static int connect_to_ip(const int sock, const char* ip, const int port)
    {
        sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port   = htons(port);

        if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
            return 0;

        if (connect(sock, (sockaddr*)&serv_addr, sizeof(sockaddr)) < 0)
            return 0;

        return 1;
    }

    /**
     * @brief Method for easily connecting to a given IP address.
     * 
     * @param address   String of the IP to connect to
     * @return int      Created socket for connection
     */
    static int connect_socket(const char* address)
    {
        int sock = make_socket();
        if (sock < 0) return -1;

        if (!connect_to_ip(sock, address, 1420))
            return -2;

        return sock;
    }

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