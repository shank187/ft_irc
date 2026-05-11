 #ifndef SERVER_HPP
#define SERVER_HPP

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

// Include the Client class we made earlier!
#include "Client.hpp" 

class Server {
private:
    int                         _port;
    int                         _server_fd;
    std::string                 _pw;
    std::vector<struct pollfd>  _fds;        // The dynamic list of buzzers
    std::map<int, Client>       _clients;   // The database of connected users

    // Private Helper Methods (To keep the main loop clean)
    void    _acceptNewClient();
    void    _handleClientMessage(int fd, char *buffer);
    void    _handleClientDisconnection(int fd);

public:
    Server(int port);
    ~Server();

    void    init(); // Handles socket(), bind(), and listen()
    void    run();  // Handles the infinite poll() loop
};

#endif