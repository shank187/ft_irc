#ifndef CORE_HPP
#define CORE_HPP

#include <map>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"
#include "mssg.hpp"

class Core {
private:
    std::map<int, Client*>          clients;
    std::map<std::string, Channel*> channels;
    std::string _server_password; 
    mssg        parse_msg(std::string line);

public:
    Core();
    Core(const Core& src);
    Core& operator=(const Core& rhs);
    ~Core();

    void        set_password(const std::string & pw);
    void        on_client_connect(int fd);
    void        on_client_disconnect(int fd);
    void        process_input(int fd, std::string text);

    void        cmd_join(Client* client, mssg& msg);
    void        cmd_privmsg(Client* client, mssg& msg);
    void        cmd_part(Client* client, mssg& msg);
    void        cmd_kick(Client* client, mssg& msg);

    void        cmd_pass(Client *client, mssg& msg);
    void        cmd_nick(Client *client, mssg& msg);
    void        cmd_user(Client *client, mssg& msg);



};

#endif