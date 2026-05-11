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

void Server::_acceptNewClient()
{
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int client_fd = accept(_server_fd,
        (struct sockaddr *) &client_address, &client_len);
    if (client_fd != -1)
    {
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        std::cout << GREEN << "A new client walked in! FD:" << RESET << client_fd << std::endl;
        struct pollfd client_pollfd;
        client_pollfd.fd = client_fd;
        client_pollfd.events = POLLIN;
        client_pollfd.revents = 0;
        _fds.push_back(client_pollfd);
        _clients[client_fd] = Client(client_fd);
    }
}

void Server::_handleClientMessage(int fd, char *buffer)
{
    _clients[fd].append_buffer(buffer);
    size_t pos;
    while((pos = _clients[fd].get_buffer().find('\n')) != std::string::npos)
    {
        std::string complete_msg = _clients[fd].get_buffer().substr(0, pos+ 1);
        std::cout << complete_msg << std::endl;
        _clients[fd].extract_buffer(pos + 1);
    }
}

void Server::run()
{
    while(true)
    {
        int poll_count = poll(_fds.data(), _fds.size(), -1);
        if(-1 == poll_count)
            throw std::runtime_error("Poll error!");
        for(size_t i = 0; i < _fds.size(); i++)
        {
            if(_fds[i].revents & POLLIN)
            {
                if(_fds[i].fd == _server_fd)
                    _acceptNewClient();
                else
                {
                    char buffer[1024];
                    std::memset(buffer, 0, sizeof(buffer));
                    int byte_received = recv(_fds[i].fd, buffer, sizeof(buffer)-1, 0);
                    if(byte_received <= 0)
                    {
                        std::cout << YELLOW <<"Client " <<_fds[i].fd << " disconnected." << std::endl;
                        _clients.erase(_fds[i].fd);
                        close(_fds[i].fd);
                        _fds.erase(_fds.begin() + i);
                        i--;
                    }
                    else
                        _handleClientMessage(_fds[i].fd,  buffer);
                }
            }
        }
    }
}
