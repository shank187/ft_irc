#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>



int main(void)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        std::cerr << "Error: failed to create socket" << std::endl;
        return 1;
    }
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

    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    std::cout << "waiting for a client to connect..." << std::endl;

    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
    if(client_fd == -1)
    {
        std::cerr << "Error: failed to accept connection." << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "A client connected! the OS gave them socket (pipe) ID: " << client_fd << std::endl;
    std::string greeting = "welcome to 1337 IRC server!\n";
    send(client_fd, greeting.c_str(), greeting.length(), 0);
    close(client_fd);
    close(server_fd);
    return 0;
}