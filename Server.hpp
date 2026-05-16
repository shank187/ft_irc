#ifndef SERVER_HPP
#define SERVER_HPP

#include "Core.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>


#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"


class Server {
    private:
        int                         _port;
        int                         _server_fd;
        std::string                 _pw;
        std::vector<struct pollfd>  _fds;
        std::map<int, std::string>      _client_buffers;
        Core                        _core;
        
        void    _acceptNewClient();
        bool    _handleClientMessage(int fd, char *buffer);
        void    _handleClientDisconnection(size_t &i);
        
    public:
        static bool                 Signal;
        Server(int port, const std::string & password);
        ~Server();
        void    init();
        void    run();
};

#endif