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
#include <map>



int main(void)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        std::cerr << "Error: failed to create socket" << std::endl;
        return 1;
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    struct sockaddr_in address; // nbdaw address configuration
    std::memset(&address, 0, sizeof(address)); //cleaning the garbage values
    address.sin_family = AF_INET; // ipv4, we are using internet protocls, not bluethoot or local Unix pipes
    address.sin_addr.s_addr = INADDR_ANY;//tells the server to listen to all available netw0rk interfaces
    address.sin_port = htons(6667);// (host to network short) tranlslate the specific port number into universal "network byte order"

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
    std::vector<struct pollfd> fds;// create a dynamically sizing list to hold pollfd structs.

    struct pollfd server_pollfd;// the specific struct the os require to monitor socket
    server_pollfd.fd = server_fd;// passing our main server door to it
    server_pollfd.events = POLLIN; // "poll inbound",only wake up the server when data comes IN
    server_pollfd.revents = 0;//"returned events" when ll put our server asleep, it monitors the sockets, 
    //when it wakes up the operating system automatically changes the value of revent to tell exactly what
    //happened, by setting it to 0 when ensure we dont accidentaly read old data from previous loop.
    fds.push_back(server_pollfd);
    std::map<int, std::string> client_buffer;
    while(true)
    {
        // 1. Wait for someone to knock or send a message
        int poll_count = poll(&fds[0], fds.size(), -1);
        if(poll_count == -1)
        {
            std::cerr << "poll error!" << std::endl;
            break;
        }
        
        // 2. Someone woke us up! Let's find out WHO it was.
        // We loop through all the buzzers we currently have.
        for (size_t i = 0; i < fds.size(); i++) 
        {
            // The OS sets POLLIN in 'revents' if this specific socket has data ready
            if (fds[i].revents & POLLIN) 
            {
                // CASE A: The buzzer that went off is our main server door!
                if (fds[i].fd == server_fd) 
                {
                    struct sockaddr_in client_address;
                    socklen_t client_len = sizeof(client_address);
                    
                    // OPEN THE DOOR! (This clears the POLLIN state so it stops infinitely looping)
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
                    
                    if (client_fd != -1) 
                    {
                        std::cout << "Success! A new client walked in! FD: " << client_fd << std::endl;
                        
                        // Rule 1: Make the new client non-blocking too!
                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        
                        // Rule 2: Give the new client their own buzzer and add them to our list
                        struct pollfd client_pollfd;
                        client_pollfd.fd = client_fd;
                        client_pollfd.events = POLLIN; // Tell poll() to watch for their messages
                        client_pollfd.revents = 0;
                        fds.push_back(client_pollfd);
                    }
                }
                // CASE B: A Client Table Buzzed (They sent a message)
                else {
                    char buffer[1024];
                    std::memset(buffer, 0, sizeof(buffer)); // Always clean the buffer first!

                    // 1. Take the order (read from the socket)
                    int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                    // 2. Did they leave the restaurant? (Ctrl+C or disconnect)
                    if (bytes_received <= 0) {
                        std::cout << "Client " << fds[i].fd << " disconnected." << std::endl;
                        
                        close(fds[i].fd);               // Close their specific socket
                        fds.erase(fds.begin() + i);     // Remove their buzzer from the list
                        i--;                            // Adjust the loop index since we shrank the list!
                    } 
                    // 3. They actually said something!
                    else {
                        client_buffer[fds[i].fd] += buffer;
                        size_t pos;
                        while((pos = client_buffer[fds[i].fd].find('\n')) != std::string::npos)
                        {
                            std::string complete_command = client_buffer[fds[i].fd].substr(0, pos + 1);
                            std::cout << "complete_command received: " << complete_command;
                            std::string reply = "Server heard you!\n";
                            send(fds[i].fd, reply.c_str(), reply.length(), 0);
                            client_buffer[fds[i].fd].erase(0, pos + 1);
                        }
                    }
                }
            }
        }
    }    
    close(server_fd);
    return 0;
}