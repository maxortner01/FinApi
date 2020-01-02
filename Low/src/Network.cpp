#include "finapi/finapi.h"

#define _FIN_DEBUG
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
    object::~object()
        { std::free(_addr); close(_socket); }

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
            { close(sock); return r; }
        
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

        close(sock);

        return r;

    #endif
    }

    int make_socket()
    {
        WSAStart();
        return socket(AF_INET, SOCK_STREAM, 0);
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
        
        sockaddr_in* addr;
        for (int lower = 9000; lower <= 10000 && !addr; lower++)
            addr = bind_socket(sock, lower);

        if (!addr) 
        {
        #ifdef _FIN_DEBUG
            std::cout << "Failed to bind a port!\n";
        #endif
            return 0;
        }

        if (connect(sock, (sockaddr*)&serv_addr, sizeof(sockaddr)) < 0)
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
            close(sock);
            return -2;
        }
        
        return sock;
    }
}

namespace Cloud
{
    /*          File             */
    File::File(Status s) :
        iterator(0), filesize(0), status(s), buffer(nullptr)
    {   }

    File::File(c_uint size) :
        iterator(0), status(OK), filesize(size), buffer( CHAR_ALLOC(size) )
    { set_ok(true); }

    void File::read(void* ptr, c_uint size)
    {
        std::memcpy(ptr, buffer + iterator, size);
        iterator += size;
    }

    File::~File()
    { std::free(buffer); }

    /*      ServerStream         */
    ServerStream::ServerStream(const char* filename, Cloud::Address address) :
        fname(filename)
    {
        _socket = network::connect_socket( ADDR_FROM_ENUM(address) );

        if (make_request("LOGIN ADMIN ADMIN123", _socket) == "OK")
            set_ok(true);
    }

    void ServerStream::read(char* dest, c_uint size) const
    {
        assert(ok());

        // Ugly concatenation to make a request to the server with the command:
        // STRM 'filename' 'byte-length'
        // and copy it over to the destination buffer passed as an argument.
        strcpy(dest, make_request( network::str_concat(
            "STRM ", fname, " ", std::to_string(size).c_str()).c_str(), 
            _socket).c_str()
        );
    }

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
        logmsg_micros("Request made and received in ");
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
        close(socket);
        return r;
    }

    bool file_exists(const char* filename, const char* address)
    {
        return (make_request(network::str_concat("exists ", filename).c_str(), address) == "T");
    }

    void request_file(const char* filename, const int i, const int filesize, char* buffer, const char* address)
    {
        int sock = network::connect_socket(address);

        if (make_request("LOGIN ADMIN ADMIN123", address) != "OK")
            { close(sock); return; }

        std::string command = network::str_concat("REQ ", filename, " ", std::to_string(i));
        make_request(command.c_str(), sock, buffer + (_FIN_BUFFER_SIZE * i), filesize);
        
        close(sock);
    }

    void get_file(const char* filename, const char* address, File*& file)
    {
        time_point(start);

        int sock = network::connect_socket(address);

        if (sock < 0)
            { file = new File(SOCKET_FAIL); return; }

        if (make_request(network::str_concat("exists ", filename).c_str(), sock) == "F")
            { close(sock); file = new File(DNE); return; }

        unsigned int filesize, chunks;
        make_request(network::str_concat("SZE ", filename).c_str(), sock, (char*)&filesize, sizeof(unsigned int));
        make_request(network::str_concat("CHK ", filename).c_str(), sock, (char*)&chunks,   sizeof(unsigned int));
        filesize -= 1;

        close(sock);

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

    void get_file(const char* filename, Address address, File*& file)
    {
        get_file(filename, _ADDR::addresses[address], file);
    }
}
}