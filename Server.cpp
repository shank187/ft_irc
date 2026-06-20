#include "Server.hpp"
#include "Client.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <iostream>


bool Server::Signal = false;

Server::Server(int port, const std::string & password) : _port(port), _pw(password) {
    _core.set_password(_pw);
}

Server::~Server() {}

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
    std::cout << GREEN << "Success! The server is listening on port " << _port << RESET << std::endl;
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
        
        struct pollfd client_pollfd;
        client_pollfd.fd = client_fd;
        client_pollfd.events = POLLIN;
        client_pollfd.revents = 0;
        _fds.push_back(client_pollfd);
        _core.on_client_connect(client_fd);

        Client* new_client = _core.get_client(client_fd);
        if (new_client)
        {
            std::string extracted_ip = inet_ntoa(client_address.sin_addr);
            new_client->set_hostname(extracted_ip);
        }
    }
}

bool Server::_handleClientMessage(int fd, char *buffer)
{
    _client_buffers[fd] += buffer;
    size_t pos;

    Client * client = _core.get_client(fd);
    if(client){
        client->update_last_activity();
    }
    while((pos = _client_buffers[fd].find('\n')) != std::string::npos)
    {
        std::string complete_msg = _client_buffers[fd].substr(0, pos + 1);
        _client_buffers[fd].erase(0, pos + 1); 

        if (complete_msg.length() > 512) continue; 

        if(! _core.process_input(fd, complete_msg))
            return false;
    }
    
    if (_client_buffers[fd].length() > 2048) _client_buffers[fd].clear();
    
    return true;
}

    
void Server::_handleClientDisconnection(size_t &i)
{
    _core.on_client_disconnect(_fds[i].fd);
    _client_buffers.erase(_fds[i].fd); 
    close(_fds[i].fd);
    _fds.erase(_fds.begin() + i);
    i--;
}

void Server::_checkPingTimeouts()
{
    time_t current_time = time(NULL);

    for (size_t i = 0; i < _fds.size(); i++) {
        if (_fds[i].fd == _server_fd) continue;

        Client* client = _core.get_client(_fds[i].fd);
        if (!client) continue;
        if (!client->get_is_auth() && (current_time - client->get_last_activity() > 60))
        {
            // kicking users if they havent logged in after 60 seconds of connecting
            _handleClientDisconnection(i);
            i--;
            continue;
        }
        // If the client has been silent for 2 minutes
        if (current_time - client->get_last_activity() > 120) 
        {
            if (!client->is_waiting_for_pong()) {
                // Phase 1: They have been idle,we Send them a PING to check if they are alive
                std::string ping_msg = "PING :ft_irc_server\r\n";
                client->set_write_buffer(ping_msg);
                client->set_waiting_for_pong(true);
            } 
            else if (current_time - client->get_last_activity() > 180) {
                // Phase 2: We sent a PING 60 seconds ago and they NEVER answered
                // Its time to kill the connection
                std::cout << "Client FD " << _fds[i].fd << " Ping Timeout. Disconnecting..." << std::endl;
                _handleClientDisconnection(i);              
                i--;
            }
        }
    }
}

void Server::_checkForOutgoingMsg()
{
    for (size_t i = 0; i < _fds.size(); i++) {
        if (_fds[i].fd != _server_fd) {
            Client* client = _core.get_client(_fds[i].fd);
            if (client && !client->get_write_buffer().empty()) {
                _fds[i].events = POLLIN | POLLOUT;
            } else {
                _fds[i].events = POLLIN;
            }
        }
    }
}

void Server::send_message(size_t &i)
{
    Client* client = _core.get_client(_fds[i].fd);
    if (client) 
    {
        std::string to_send = client->get_write_buffer();
        ssize_t bytes_sent = send(_fds[i].fd, to_send.c_str(), to_send.length(), 0);                    
        if (bytes_sent > 0) {
            client->erase_from_write_buffer(bytes_sent);
        } else if (bytes_sent <= 0) {
            if(!(bytes_sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)))
                _handleClientDisconnection(i);
        }
    }
}

void Server::run()
{
    while(true)
    {
        _checkPingTimeouts();
        _checkForOutgoingMsg();
        int poll_count = poll(_fds.data(), _fds.size(), 5000);
        if(-1 == poll_count)
        {
            if(Server::Signal == true)
                break;
            else
                throw std::runtime_error("Poll error!");
        }
        else if(poll_count == 0)
            continue;
        for(size_t i = 0; i < _fds.size(); i++)
        {
            bool is_disconnected = false;
            if(_fds[i].revents & POLLIN)
            {
                if(_fds[i].fd == _server_fd)
                    _acceptNewClient();
                else
                {
                    char buffer[1024];
                    std::memset(buffer, 0, sizeof(buffer));
                    int byte_received = recv(_fds[i].fd, buffer, sizeof(buffer)-1, 0);
                    if(byte_received == 0 || (byte_received == -1 && errno != EAGAIN
                    && errno != EWOULDBLOCK)) {
                        is_disconnected = true;
                        _handleClientDisconnection(i);
                    }
                    else if (!_handleClientMessage(_fds[i].fd, buffer))
                    {
                        is_disconnected = true;
                        _handleClientDisconnection(i);
                    }
                }
            }
            if(is_disconnected)
                continue;
            if (_fds[i].revents & POLLOUT)
            {
                send_message(i);
            }
        }
    }
    for (size_t i = 0; i < _fds.size(); i++) {
        close(_fds[i].fd);
    }
    std::cout << GREEN << "All connections closed... \n"<< YELLOW << "Server shutting down." << RESET <<std::endl;
}