#pragma once

#include "Network.h"

#include <vector> // vector
#include <thread> // thread

#pragma comment(lib, "pthread")

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

#   define time_point(...)
#   define logmsg_micros(...)
#   define logmsg_ms(...)

#endif

namespace finapi
{
namespace Cloud
{
namespace _ADDR
{
    static const char* addresses[] = {
        "127.0.0.1",
        "192.168.1.128"
    };
}

    enum Address
    {
        LocalHost,
        LocalServer
    };

    enum Status
    {
        OK,
        DNE,
        EMPTY,
        SOCKET_FAIL,
        CONNECT_FAIL
    };

    struct File
    {
        Status status;
        const unsigned int filesize;
        char* buffer;

        File(Status s = EMPTY) : 
            filesize(0), status(s), buffer(nullptr)
        {   }

        File(const unsigned int size) :
            status(OK), filesize(size), buffer( (char*)std::calloc(size, 1) )
        {   } 

        ~File()
        {  std::free(buffer);  }
    };

    /**
     * @brief Make a request to the server with a given command.
     * 
     * @param command   Command to send to the server
     * @param socket    Socket of the server
     * @param buffer    Location of the buffer to populate
     * @param size      Bytesize of the given buffer
     */
    static void make_request(const char* command, const int socket, char* buffer, int size = -1)
    {
        // Get start time for debugging
        time_point(start);

        // Set the size of the client buffer
        if (size == -1) size = _FIN_BUFFER_SIZE;

        // Send the command to the server
        send(socket, command, strlen(command), 0);

        // Zero out the client buffer and receive incoming info
        std::memset(buffer, 0, size);
        recv(socket, buffer, size, 0);

        time_point(stop);
        logmsg_micros("Request made and received in ");
    }

    static std::string make_request(const char* command, const int socket)
    {
        char* buffer = (char*)std::malloc(_FIN_BUFFER_SIZE);
        make_request(command, socket, buffer);
        std::string r = buffer;
        std::free(buffer);
        return r;
    }

    /**
     * @brief Create a socket solely for the purpose of pulling a file from the server
     * 
     * @param filename  Name of the file to pull
     * @param i         Index of the chunk within the file to pull
     * @param filesize  Size of the file
     * @param buffer    Location of the buffer to populate       
     * @param address   IP Address of the server
     */
    static void request_file(const char* filename, const int i, const int filesize, char* buffer, const char* address)
    {
        int sock = network::connect_socket(address);

        std::string command = network::str_concat("REQ ", filename, " ", std::to_string(i));
        make_request(command.c_str(), sock, buffer + (_FIN_BUFFER_SIZE * i), filesize);
        
        // Close socket
    }
    
    static void get_file(const char* filename, const char* address, File*& file)
    {
        time_point(start);

        int sock = network::connect_socket(address);
        
        if (sock < 0)
            { file = new File(SOCKET_FAIL); return; }

        if (make_request(network::str_concat("exists ", filename).c_str(), sock) == "F")
            { file = new File(DNE); return; }

        unsigned int filesize, chunks;
        make_request(network::str_concat("SZE ", filename).c_str(), sock, (char*)&filesize, sizeof(unsigned int));
        make_request(network::str_concat("CHK ", filename).c_str(), sock, (char*)&chunks,   sizeof(unsigned int));
        filesize -= 1;

        // Close socket

        file = new File(filesize);

        std::vector<std::thread*> threads;
        threads.reserve(chunks);

        for (int i = 0; i < chunks; i++)
            threads.push_back(new std::thread(request_file, filename, i, filesize, file->buffer, address));

    #ifdef _FIN_DEBUG
        std::cout << "There are " << threads.size() << " threads.\n";
    #endif

        for (int i = 0; i < threads.size(); i++)
        {
            threads[i]->join();
            delete threads[i];
        }

        *(file->buffer + file->filesize) = '\0';

        time_point(stop);
        logmsg_ms("File received in ");
    }

    static void get_file(const char* filename, Address address, File*& file)
    {
        get_file(filename, _ADDR::addresses[address], file);
    }
}
}