#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ctime>

bool send_all(int fd, const std::string& data) {
    size_t total_sent = 0;
    size_t bytes_left = data.length();
    while (total_sent < data.length()) {
        ssize_t n = send(fd, data.c_str() + total_sent, bytes_left, 0);
        if (n == -1) return false;
        total_sent += n;
        bytes_left -= n;
    }
    return true;
}

int main(int ac, char **av) {
    if (ac != 4) return (std::cerr << "Usage: ./bot <IP> <PORT> <PASS>\n", 1);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return (std::cerr << "Error: Socket creation failed!\n", 1);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::atoi(av[2]));
    server_addr.sin_addr.s_addr = inet_addr(av[1]);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return (std::cerr << "Error: Connection failed!\n", close(sock), 1);

    std::cout << "Bot connected successfully!\n";

    std::string auth = "PASS " + std::string(av[3]) + "\r\nNICK TimeBot\r\nUSER bot 0 * :Time Bot\r\n";
    send_all(sock, auth);

    char buf[1024];
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        if (recv(sock, buf, 1023, 0) <= 0)
            break;

        std::string msg(buf);
        std::cout << ">> " << msg;

        if (msg.find("PING") == 0)
        {
            size_t r_pos = msg.find('\r');
            if (r_pos != std::string::npos)
            {
                std::string pong = "PONG " + msg.substr(5, r_pos - 5) + "\r\n";
                send_all(sock, pong);
            }
        }
        else if (msg.find("PRIVMSG") != std::string::npos && msg.find("!time") != std::string::npos)
        {
            size_t excl_pos = msg.find('!');
            size_t privmsg_pos = msg.find(" PRIVMSG ");

            if (msg[0] == ':' && excl_pos != std::string::npos && privmsg_pos != std::string::npos && excl_pos < privmsg_pos)
            {
                std::string sender_nick = msg.substr(1, excl_pos - 1);

                size_t target_start = privmsg_pos + 9;
                size_t target_end = msg.find(' ', target_start);

                if (target_end != std::string::npos)
                {
                    std::string incoming_target = msg.substr(target_start, target_end - target_start);

                    time_t now = time(NULL);
                    std::string dt = ctime(&now);
                    if (!dt.empty() && dt[dt.length() - 1] == '\n')
                        dt.erase(dt.length() - 1);

                    std::string target;
                    if (!incoming_target.empty() && incoming_target[0] == '#') 
                    {
                        target = incoming_target;
                    }
                    else 
                    {
                        target = sender_nick;
                    }

                    std::string reply = "PRIVMSG " + target + " :Server time: " + dt + "\r\n";
                    send_all(sock, reply);
                }
            }
        }
    }
    
    std::cout << "Disconnected from server!\n";
    return (close(sock), 0);
}