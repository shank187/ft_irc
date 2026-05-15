#include "Core.hpp"
#include <iostream>
#include <string>
#include <cctype>
#include "Numeric.hpp"

Core::Core() {}

Core::Core(const Core& src)
{
    *this = src;
}

Core& Core::operator=(const Core& rhs)
{
    if (this != &rhs) {
        this->clients = rhs.clients;
        this->channels = rhs.channels;
    }
    return *this;
}

Core::~Core() {
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();

    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        delete it->second;
    }
    channels.clear();
}

void Core::on_client_connect(int fd) {
    Client* new_client = new Client(fd);
    clients[fd] = new_client;
    std::cout << "[Core] Client connected on FD: " << fd << std::endl;
}

void Core::on_client_disconnect(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd);
    
    if (it != clients.end()) {
        Client* client_to_remove = it->second;

        std::map<std::string, Channel*>::iterator chan_it;
        for (chan_it = channels.begin(); chan_it != channels.end(); ++chan_it)
        {
            chan_it->second->remove_client(client_to_remove);
        }

        delete client_to_remove;
        clients.erase(it);
        
        std::cout << "[Core] Client disconnected on FD: " << fd << std::endl;
    }
}

void Core::set_password(const std::string & pw) {
    _server_password = pw;
}

void Core::cmd_pass(Client *client, mssg& msg)
{
    if (client->get_has_password())
        return;

    std::string nick = client->get_nickname().empty() ? "*" : client->get_nickname();

    if (msg.args.size())
    {
        if(msg.args[0] == _server_password)
        {
            client->set_has_password(true);
            client->reply("Correct! password check passed\r\n");
            std::cout << GREEN << "Client fd: " << client->get_fd() << ", has passed password check." << RESET << std::endl;            
        }
        else
        {
            client->reply(ERR_PASSWDMISMATCH(nick));
            std::cout << YELLOW << "Client fd: " << client->get_fd() << ", has entered an incorrect pw." << RESET << std::endl;
        }
    }
    else
    {
        client->reply(ERR_NEEDMOREPARAMS(nick, "PASS"));
    }
}

bool Core::validate_nickname(const std::string &nick)
{
    if(nick.empty() || nick.size() > 9)
        return false;
    
    if (std::isdigit(nick[0]))
        return false;
        
    for(size_t i = 0; i < nick.size(); i++)
    {
        char c = nick[i];
        bool is_special = (c == '_' || c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}');
        
        if (!std::isalnum(c) && !is_special)
            return false;
    }
    return true;
}

bool Core::check_is_nick_exist(const std::string & nickname)
{
    std::map<int, Client *>::iterator it;
    for(it = clients.begin(); it != clients.end(); it++)
    {
        Client *client = it->second;
        if(!client) continue;
        
        if(client->get_nickname() == nickname)
            return true; 
    }
    return false;
}

void Core::cmd_nick(Client *client, mssg& msg)
{

    if (!client->get_has_password())
    {
        client->reply("464 :Please provide the server password first (PASS <password>)\r\n");
        return;
    }
    std::string nick = client->get_nickname().empty() ? "*" : client->get_nickname();

    if(msg.args.empty())
    {
        client->reply(ERR_NONICKNAMEGIVEN(nick));
        return;
    }
    if(!validate_nickname(msg.args[0]))
    {
        
        client->reply(ERR_ERRONEUSNICKNAME(nick, msg.args[0]));
        return;
    }
    if(check_is_nick_exist(msg.args[0]))
    {
        client->reply(ERR_NICKNAMEINUSE(nick, msg.args[0]));
        return;
    }    
    
    client->set_nickname(msg.args[0]);
    client->reply("Your nickname successfully set to: " + msg.args[0] + "\r\n");
    // WELCOME CHECK
    if (client->get_is_auth()) {
        client->reply(RPL_WELCOME(client->get_nickname()));
    }
}

void Core::cmd_user(Client *client, mssg& msg)
{
    if (!client->get_has_password()) {
        client->reply("464 :Please provide the server password first (PASS <password>)\r\n");
        return;
    }
    if (client->get_is_auth()) 
    {
        client->reply(ERR_ALREADYREGISTRED(client->get_nickname()));
        return;
    }

    if (msg.args.size() < 4) 
    {
        client->reply(ERR_NEEDMOREPARAMS(client->get_nickname(), "USER"));
        return;
    }
    client->set_username(msg.args[0]);
    client->set_realname(msg.args[3]);

    if (client->get_is_auth()) 
    {
        client->reply(RPL_WELCOME(client->get_nickname()));
        std::cout << GREEN << "Client fd: " << client->get_fd() << " is fully registered!" << RESET << std::endl;
    }
}

void Core::cmd_join(Client* client, mssg& msg) {
    if (msg.args.empty())
    {
        client->reply(ERR_NEEDMOREPARAMS(client->get_nickname(), "JOIN"));
        return;
    }

    std::string chan_name = msg.args[0];
    
    std::map<std::string, Channel*>::iterator it = channels.find(chan_name);
    Channel* channel;

    if (it == channels.end())
    {
        channel = new Channel(chan_name);
        channels[chan_name] = channel;
    } else
        channel = it->second;
    channel->add_client(client);
    std::string join_msg = ":" + client->get_nickname() + " JOIN :" + chan_name + "\r\n";
    channel->broadcast(join_msg, NULL);
}

void Core::cmd_privmsg(Client* client, mssg& msg)
{
    if (msg.args.size() < 2)
    {
        client->reply(ERR_NOTEXTTOSEND(client->get_nickname()));
        return;
    }

    std::string target = msg.args[0];
    std::string text = msg.args[1];

    std::map<std::string, Channel*>::iterator it = channels.find(target);
    
    if (it != channels.end())
    {
        std::string full_msg = ":" + client->get_nickname() + " PRIVMSG " + target + " :" + text + "\r\n";
        it->second->broadcast(full_msg, client);
    } else
    {
        client->reply(ERR_NOSUCHNICK(client->get_nickname(), target));
    }
}

// void Core::cmd_part(Client* client, mssg& msg)
// {

// }

// void Core::cmd_kick(Client* client, mssg& msg)
// {
    
// }