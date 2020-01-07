#include "finapi/finapi.h"

#ifdef _FIN_WINDOWS

    int inet_pton(int af, const char *src, void *dst)
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

#else

#   include <net/if.h>      // mac address things
#   include <sys/ioctl.h>   // ioctl

#endif

namespace finapi
{
namespace network
{
#pragma region NETWORK_H
    // The magic error that comes from nowhere. I guess this will
    // never be freed
    object::~object()
        { /* std::free(_addr); */ close_connection(_socket); }

    mac get_mac_address()
    {
    #ifdef _FIN_WINDOWS
    
    #else
        mac r;
        std::memset(r.address, 0, 6 * sizeof(unsigned char));

        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0) return r;

        char*  buffer = (char*)std::calloc(1, _FIN_BUFFER_SIZE);
        ifreq  ifr;
        ifconf ifc;

        ifc.ifc_len = _FIN_BUFFER_SIZE;
        ifc.ifc_buf = buffer;

        if (ioctl(sock, SIOCGIFCONF, &ifc) == -1)
            { close_connection(sock); return r; }
        
        ifreq* it = ifc.ifc_req;
        const ifreq* end = it + (ifc.ifc_len / sizeof(ifreq));

        bool success = false;
        do
        {
            strcpy(ifr.ifr_name, it->ifr_name);
            if ( ioctl(sock, SIOCGIFFLAGS, &ifr) == 0 )
            {
                if (!(ifr.ifr_flags & IFF_LOOPBACK))
                {
                    if ( ioctl(sock, SIOCGIFHWADDR, &ifr) == 0 )
                    {
                        success = true;
                        break;
                    }
                }
            }
            else
            {
                break;
            }

            it++;
        } while (it != end);
        
        if (success)
            std::memcpy(r.address, ifr.ifr_hwaddr.sa_data, 6);

        close_connection(sock);

        return r;

    #endif
    }

    int make_socket()
    {
        WSAStart();
        return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    int set_options(const int sock) 
    {
        int opt;

        if ( setsockopt(sock, SOL_SOCKET, SOCK_OPT, (char*)&opt, sizeof(int)) )
            return 0;
        return 1;
    }

    sockaddr_in* bind_socket(const int sock, const int port)
    {
        sockaddr_in* address = new sockaddr_in;
        int sock_len = sizeof(address);

        address->sin_family = AF_INET;
        address->sin_addr.s_addr = INADDR_ANY;
        address->sin_port = htons(port);

    #ifdef _FIN_DEBUG
        std::cout << "Binding socket " << sock << " to port " << port << "\n";
    #endif

        if ( bind(sock, (struct sockaddr*)address, sizeof(sockaddr_in)) != 0 )
        {
            std::free(address);
            return nullptr;
        }

        return address;
    } 

    int make_listen(const int sock, const int queue)
    {
        if (listen(sock, queue) < 0)
            return 0;
        return 1;
    }

    int accept_socket(const int sock, sockaddr_in* in, int* addr_len)
    {
        return accept(sock, (sockaddr*)in, (socklen_t*)addr_len);
    }

    int connect_to_ip(const int sock, const char* ip, const int port)
    {
        sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port   = htons(port);

        if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
            return 0;
        
        /*
        sockaddr_in* addr;
        for (int lower = 9000; lower <= 10000 && !addr; lower++)
            addr = bind_socket(sock, lower);

        if (!addr) 
        {
        #ifdef _FIN_DEBUG
            std::cout << "Failed to bind a port!\n";
        #endif
            return 0;
        } */

        if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        {
        #ifdef _FIN_DEBUG
            std::cout << "Connection failed with error code " << errno << ": " << strerror(errno) << "\n";
        #endif
            return 0;
        }

        return 1;
    }

    int connect_socket(const char* address)
    {
        int sock = make_socket();
        if (sock < 0) return -1;

        if (!connect_to_ip(sock, address, _FIN_PORT))
        {
            close_connection(sock);
            return -2;
        }
        
        return sock;
    }

#pragma endregion
}

namespace Cloud
{
#pragma region FILE_H

#pragma region FILE

    File::File() :
        iterator(0), fsize(0), _status(EMPTY), _buffer(nullptr)
    {   }

    File::File(const char* filename, const char* address) :
        _status( get_file(filename, address, _buffer, &fsize, false) ), iterator(0)
    {   
        if (_status == OK) set_ok(true);
    }

    File::File(const char* filename, Cloud::Address address) :
        File(filename, ADDR_FROM_ENUM( address ))
    {   }

    ReadStatus File::read(char* ptr, c_uint size)
    {
        if (iterator >= fsize)
            return ReadStatus(false);

        std::memcpy(ptr, _buffer + iterator, size);
        iterator += size;

        return ReadStatus();
    }

    void File::close()
    {
        if (good())
        {
            std::free(_buffer);
            _buffer = nullptr;
            set_ok(false);
        }
    }

    const Status File::status() const
    {
        return _status;
    }

    const uint File::filesize() const 
    {
        return fsize;
    }

    const uint File::position() const
    {
        return iterator;
    }

    File::~File()
    { close(); }

#pragma endregion

#pragma region FILE_FRIEND

    FileFriend::FileFriend(File& file) :
        _file(&file)
    {   }

    ReadStatus FileFriend::read(char* buffer, c_uint size) 
    {
        return _file->read(buffer, size);
    }

    const bool FileFriend::good() const 
    {
        if (!_file->_buffer) return false;

        return _file->good();
    }

#pragma endregion

#pragma endregion

#pragma region SERVER_STREAM_H

    ServerStream::ServerStream(const char* filename, const char* address) :
        fname(filename), filesize(0), it(0)
    {
        _socket = network::connect_socket( address );
        assert(_socket > 0);
        if (make_request("LOGIN ADMIN ADMIN123", _socket) == "OK")
            set_ok(true);

        if (make_request(network::str_concat("exists ", filename).c_str(), _socket) == "F")
        {
            set_ok(false);
            return;
        }

        make_request(
            network::str_concat("SZE ", filename).c_str(), 
            _socket,
            (char*)&filesize,
            sizeof(unsigned int)
        );
    }

    ServerStream::ServerStream(const char* filename, Cloud::Address address) :
        ServerStream(filename, ADDR_FROM_ENUM(address))
    {   }

    ReadStatus ServerStream::read(char* dest, uint size)
    {
        assert(good());

        // If the end of the block is after the end of the file,
        // change the size to accomidate for it
        if (it + size > filesize) size = filesize - (it + size);

        // If the iterator is at the end of the file return failed read
        if (it >= filesize || size == 0) return ReadStatus(false);

        // Ugly concatenation to make a request to the server with the command:
        // STRM 'filename' 'byte-length'
        // and copy it over to the destination buffer passed as an argument.
        std::memset(dest, 0, size);
        make_request(network::str_concat(
            "STRM ", fname, " ", std::to_string(size).c_str(), " ", std::to_string(it)).c_str(),
            _socket,
            dest,
            size
        );
        
        it += size;

        return ReadStatus();
    }

    void ServerStream::seek(c_uint bytes)
    {
        it = bytes;
    }

#pragma endregion

#pragma region CCLIENT_H
    void make_request(const char* command, const int socket, char* buffer, int size)
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
    }

    std::string make_request(const char* command, const int socket)
    {
        char* buffer = (char*)std::malloc(_FIN_BUFFER_SIZE);
        make_request(command, socket, buffer);
        std::string r = buffer;
        std::free(buffer);
        return r;
    }

    std::string make_request(const char* command, const char* address)
    {
        int socket = network::connect_socket(address);
        std::string r = make_request(command, socket);
        close_connection(socket);
        return r;
    }

    bool file_exists(const char* filename, const char* address)
    {
        return (make_request(network::str_concat("exists ", filename).c_str(), address) == "T");
    }

    void request_file(const char* filename, const int i, const int filesize, char* buffer, const char* address)
    {
        static unsigned int read = 0;

        unsigned int block_size = _FIN_BUFFER_SIZE;
        c_uint END_BLOCK = _FIN_BUFFER_SIZE * (i + 1);

        if (END_BLOCK > filesize)
            block_size = filesize - (_FIN_BUFFER_SIZE * i);

        Cloud::ServerStream stream(filename, address);
        stream.seek(i * _FIN_BUFFER_SIZE);
        stream.read(buffer + (_FIN_BUFFER_SIZE * i), block_size);
    }

    Status get_file(const char* filename, const char* address, char*& buffer, uint* fsize, bool threaded)
    {
        time_point(start);

        int sock = network::connect_socket(address);

        if (sock < 0) return SOCKET_FAIL;

        if (make_request(network::str_concat("exists ", filename).c_str(), sock) == "F")
            { close_connection(sock); return DNE; }

        unsigned int filesize, chunks;
        make_request(network::str_concat("SZE ", filename).c_str(), sock, (char*)&filesize, sizeof(unsigned int));
        make_request(network::str_concat("CHK ", filename).c_str(), sock, (char*)&chunks,   sizeof(unsigned int));
        
        *fsize = filesize;
        
        close_connection(sock);

        buffer = (char*)std::malloc(filesize);
        std::memset(buffer, 0, filesize);

        std::vector<std::thread*> threads;
        threads.reserve(chunks);

        for (int i = 0; i < chunks; i++)
        {
            if (threaded)
                threads.push_back(new std::thread(request_file, filename, i, filesize, buffer, address));
            else
                request_file(filename, i, filesize, buffer, address);
        }

        for (int i = 0; i < threads.size(); i++)
        {
            threads[i]->join();
            delete threads[i];
        }

        //*(buffer + filesize) = '\0';

        time_point(stop);
    }

    Status get_file(const char* filename, Address address, char*& buffer, uint* fsize, bool threaded)
    {
        return get_file(filename, _ADDR::addresses[address], buffer, fsize, threaded);
    }

#pragma endregion
}
}