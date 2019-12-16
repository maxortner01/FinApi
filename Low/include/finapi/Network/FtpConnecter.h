#pragma once

#include <iostream>

#ifdef __WIN32__
#   include <WinSock2.h>
#   include <WS2tcpip.h>
#   include <chrono>
#   include <thread>

#   define usleep(micro) std::this_thread::sleep_for(std::chrono::milliseconds(micro))
#else
#   include <sys/socket.h>
#   include <unistd.h>
#endif


#define BUFFER_SIZE 1024
#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

namespace network
{
    int inet_pton4(const char *src, char *dst)
    {
        uint8_t tmp[NS_INADDRSZ], *tp;

        int saw_digit = 0;
        int octets = 0;
        *(tp = tmp) = 0;

        int ch;
        while ((ch = *src++) != '\0')
        {
            if (ch >= '0' && ch <= '9')
            {
                uint32_t n = *tp * 10 + (ch - '0');

                if (saw_digit && *tp == 0)
                    return 0;

                if (n > 255)
                    return 0;

                *tp = n;
                if (!saw_digit)
                {
                    if (++octets > 4)
                        return 0;
                    saw_digit = 1;
                }
            }
            else if (ch == '.' && saw_digit)
            {
                if (octets == 4)
                    return 0;
                *++tp = 0;
                saw_digit = 0;
            }
            else
                return 0;
        }
        if (octets < 4)
            return 0;

        memcpy(dst, tmp, NS_INADDRSZ);

        return 1;
    }
}

namespace finapi
{
    class FtpConnector
    {
        SOCKET _sock;

        int         _login() const;
        std::string _send(std::string msg, bool r = false) const;
        int         _connect();

    public:
        FtpConnector();
        
        int close() const;
    };

    int FtpConnector::_login() const
    {
        const char* username = "USER admin";
        const char* password = "USER finadmin1";

        std::cout << _send(username, true) << "\n";
        _send(password);
    }

    std::string FtpConnector::_send(std::string msg, bool r) const
    {
        std::cout << "Sending msg '" << msg << "' (" << msg.length() << " bytes)...";
        if (send(_sock, msg.c_str(), msg.length(), 0) < 0)
        {
            std::cout << " failed.\n"; 
            return 0;
        }
        std::cout << " done.\n";

        if (!r) return "";

        usleep(1000);
        std::cout << "Getting response...";

        char buffer[BUFFER_SIZE] = {0};
        if (recv(_sock, buffer, BUFFER_SIZE, 0) < 0)
        {
            std::cout << " failed.\n";
            return 0;
        }

        std::string ret(buffer);

        std::cout << " done.\n";

        return ret;
    }

    int FtpConnector::_connect()
    {
    #ifdef __WIN32__

        WSADATA wsadata;
        int error = WSAStartup(0x0202, &wsadata);
        if (error)
        {
            std::cout << "Error starting WSA.\n";
            return 0;
        }

    #endif

        struct sockaddr_in serv_addr;
        _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_sock == INVALID_SOCKET)
        {
            std::cout << "Socket creation error.\n";
            return 0;
        }

        serv_addr.sin_port   = htons(2121);
        serv_addr.sin_family = AF_INET;
        if (network::inet_pton4("192.168.1.128", (char*)&serv_addr.sin_addr) <= 0)
        {
            std::cout << "Invalid Address or address not supported.\n";
            return 0;
        }

        std::cout << "Connecting to IP...";
        if (connect(_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        {
            std::cout << "Connection failed.\n";
            return 0;
        }
        std::cout << " done.\n";

        std::cout << "Reading banner...";
        char buffer[BUFFER_SIZE] = {0};
        if (recv(_sock, buffer, BUFFER_SIZE, 0) < 0)
        {
            std::cout << " failed.\n";
            return 0;
        }
        std::cout << " done.\n";
        std::cout << "  Banner: " << buffer << "\n";

        return 1;
    }

    FtpConnector::FtpConnector()
    {
        if (_connect())
            std::cout << "FTP connected successfully.\n";

        if (_login())
            std::cout << "FTP logged in successfully.\n";

        std::cout << _send("ls", true) << '\n';
    }

    int FtpConnector::close() const
    {
        _send("QUIT");
        if (closesocket(_sock) < 0)
        {
            std::cout << "Socket failed to close.\n";
            return 0;
        }

    #ifdef __WIN32__
        WSACleanup();
    #endif

        std::cout << "Disconnected from FTP.\n";
        return 1;
    }
}