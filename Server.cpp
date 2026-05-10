#include "Server.hpp"
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <iostream>



Server::Server(int port):_port(port){}

Server::~Server(){}

void Server::init()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_server_fd == -1)
        throw(std::runtime_error("failed to Create Main server's Socket."));
    fcntl(_server_fd, F_SETFL, O_NONBLOCK);
    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw(std::runtime_error("failed to set socket options."));
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);
    if(bind(_server_fd, (struct sockaddr *)& address, sizeof(address)) == -1)
        throw (std::runtime_error("failed to bind to port."));
    if(listen(_server_fd, 10) == -1)
        throw (std::runtime_error("failed to listen."));
    std::cout << GREEN << "Success! The server is listening on port " << _port <<std::endl;
    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    _fds.push_back(server_pollfd);
}

void Server::_

void Server::run()
{
    while(true)
    {
        int poll_count = poll(_fds.data(), _fds.size(), -1);
        if(-1 == poll_count)
            throw std::runtime_error("Poll error!");
        for(int i = 0; i < _fds.size(); i++)
        {
            if(_fds[i].fd == _server_fd)
                _acceptNewClient();
            else
                _handleClientMessage(_fds[i].fd);
        }

    }
}
