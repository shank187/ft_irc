#include "Server.hpp"
#include <iostream>
#include <csignal> 


bool is_valid_port(const std::string& port_str)
{
    if (port_str.empty()) 
        return false;
        
    for (size_t i = 0; i < port_str.length(); i++) {
        if (!std::isdigit(port_str[i])) {
            return false;
        }
    }
    
    int port = std::atoi(port_str.c_str());
    if (port < 1024 || port > 65535) {
        return false;
    }
    
    return true;
}

void sig_handler(int signum) {
    (void)signum;
    std::cout << "\n[!] Signal received. Shutting down server gracefully..." << std::endl;
    Server::Signal = true;
}


int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr<< RED << "Usage: ./ircserv <port> <password>" << RESET << std::endl;
        return 1;
    }
    if (!is_valid_port(argv[1]))
    {
        std::cerr << RED<<"Error: Invalid port number =>" << RESET << argv[1] << std::endl;
        return 1;
    }
    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    Server ircServer(port, password);

    signal(SIGINT, sig_handler);
    signal(SIGQUIT, sig_handler);
    try {
        ircServer.init();
        ircServer.run();
    } 
    catch (const std::exception& e) {
        std::cerr << RED << "Fatal Error: " << RESET << e.what() << std::endl;
        return 1;
    }

    return 0;
}