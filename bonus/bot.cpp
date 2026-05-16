#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ctime>

int main(int ac, char **av) {
    if (ac != 4) return (std::cerr << "Usage: ./bot <IP> <PORT> <PASS>\n", 1);

    //IPv4, tcp
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return (std::cerr << "Error: Socket creation failed!\n", 1);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::atoi(av[2]));//"Host to Network Short"
    server_addr.sin_addr.s_addr = inet_addr(av[1]);//Converts the IP address from a string (e.g., "127.0.0.1") into a 32-bit binary integer.

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return (std::cerr << "Error: Connection failed!\n", close(sock), 1);

    std::cout << "Bot connected successfully!\n";

    std::string auth = "PASS " + std::string(av[3]) + "\r\nNICK TimeBot\r\nUSER bot 0 * :Time Bot\r\n";
    send(sock, auth.c_str(), auth.length(), 0);

    char buf[1024];
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        if (recv(sock, buf, 1023, 0) <= 0)// 0 if server close connection 
            break;

        std::string msg(buf);
        std::cout << ">> " << msg;

        if (msg.find("PING") == 0) {
            std::string pong = "PONG " + msg.substr(5) + "\r\n";
            send(sock, pong.c_str(), pong.length(), 0);
        }
        else if (msg.find("PRIVMSG") != std::string::npos && msg.find("!time") != std::string::npos)
        {
            size_t pos = msg.find('!');
            if (msg[0] == ':' && pos != std::string::npos)
            {
                time_t now = time(NULL); // seconds since 1970
                std::string dt = ctime(&now);// Converts that timestamp into a human-readable string
                dt.erase(dt.length() - 1);
                std::string reply = "PRIVMSG " + msg.substr(1, pos - 1) + " :Server time: " + dt + "\r\n";
                send(sock, reply.c_str(), reply.length(), 0);
            }
        }
    }
    
    std::cout << "Disconnected from server!\n";
    return (close(sock), 0);
}