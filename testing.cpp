#include <cstddef>
#include <iostream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <vector>
#include <poll.h>


int main(void)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        std::cerr << "Error: failed to create socket" << std::endl;
        return 1;
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(6667);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) ==  -1)
    {
        close(server_fd);
        std::cerr << "Error: failed to bind to port" << std::endl;
        return 1;
    }
    if (listen(server_fd, 10) == -1){
        std::cerr << "Error: failed to listen." << std::endl;
        return 1;
    }
    std::cout << "success: the server is listening to the port 6667." << std::endl;
    std::vector<struct pollfd> fds;

    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    fds.push_back(server_pollfd);
    size_t loop_count = 0;
    while(true)
    {
        struct sockaddr_in client_address;
    }
    close(server_fd);
    return 0;
}