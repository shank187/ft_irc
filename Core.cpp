#include "Core.hpp"
#include <iostream>
#include <string>
#include <cctype>
#include "Client.hpp"
#include "Numeric.hpp"
#include "mssg.hpp"
#include <set>

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

void Core::on_client_connect(int fd)
{
    Client* new_client = new Client(fd);
    clients[fd] = new_client;
    std::cout << "[Core] Client connected on FD: " << fd << std::endl;
}

void Core::on_client_disconnect(int fd)
{
    std::map<int, Client*>::iterator it = clients.find(fd);
    
    if (it != clients.end()) {
        Client* client_to_remove = it->second;

        std::map<std::string, Channel*>::iterator chan_it = channels.begin();
        while (chan_it != channels.end())
        {
            chan_it->second->remove_client(client_to_remove);
            if (chan_it->second->is_operator(client_to_remove))
                chan_it->second->remove_operator(client_to_remove);

            if (chan_it->second->get_members().empty())
            {
                delete chan_it->second;
                channels.erase(chan_it++);
            }
            else
            {
                ++chan_it;
            }
        }

        delete client_to_remove;
        clients.erase(it);
        
        std::cout << "[Core] Client disconnected on FD: " << fd << std::endl;
    }
}


void Core::broadcast_global(Client* sender, const std::string& message, bool include_sender)
{
    std::set<Client*> recipients;
    if(include_sender)
        recipients.insert(sender);

    std::map<std::string, Channel*>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it) 
    {
        if (it->second->is_member(sender)) 
        {
            std::vector<Client*> members = it->second->get_members();
            for (size_t i = 0; i < members.size(); i++) {
                if (members[i] != sender || include_sender == true)
                    recipients.insert(members[i]);
            }
        }
    }
    std::set<Client*>::iterator set_it;
    for (set_it = recipients.begin(); set_it != recipients.end(); ++set_it) {
        (*set_it)->set_write_buffer(message);
    }
}

void Core::set_password(const std::string & pw) {
    _server_password = pw;
}

void Core::cmd_quit(Client* client, mssg& msg)
{
    std::string reason = "Client Quit";
    if (!msg.args.empty()) {
        reason = msg.args[0]; 
    }
    
    client->set_write_buffer("ERROR :Closing Link: " + client->get_nickname() + " (" + reason + ")\r\n");
    
    std::string quit_msg = ":" + client->get_nickname() + " QUIT :Quit: " + reason + "\r\n";
    broadcast_global(client, quit_msg, false);

    std::cout << YELLOW << "[Server] Client FD " << client->get_fd() << " issued QUIT (" << reason << ")" << RESET << std::endl;
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
            std::cout << GREEN << "Client fd: " << client->get_fd() << ", has passed password check." << RESET << std::endl;            
        }
        else
        {
            client->set_write_buffer(ERR_PASSWDMISMATCH(nick));
            std::cout << YELLOW << "Client fd: " << client->get_fd() << ", has entered an incorrect pw." << RESET << std::endl;
        }
    }
    else
    {
        client->set_write_buffer(ERR_NEEDMOREPARAMS(nick, "PASS"));
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
        client->set_write_buffer("464 :Please provide the server password first (PASS <password>)\r\n");
        return;
    }
    std::string nick = client->get_nickname().empty() ? "*" : client->get_nickname();

    if(msg.args.empty())
    {
        client->set_write_buffer(ERR_NONICKNAMEGIVEN(nick));
        return;
    }
    if(!validate_nickname(msg.args[0]))
    {
        
        client->set_write_buffer(ERR_ERRONEUSNICKNAME(nick, msg.args[0]));
        return;
    }
    if (msg.args[0] == client->get_nickname()) return;
    if(check_is_nick_exist(msg.args[0]))
    {
        client->set_write_buffer(ERR_NICKNAMEINUSE(nick, msg.args[0]));
        return;
    }    
    
    std::string old_nick = client->get_nickname();
    if (old_nick.empty()) {
        old_nick = "*";
    }
    client->set_nickname(msg.args[0]);
    if (client->get_is_auth()) 
    {
        if (old_nick != "*" && old_nick != msg.args[0]) 
        {
            std::string nick_msg = ":" + old_nick + " NICK :" + client->get_nickname() + "\r\n";
            broadcast_global(client ,nick_msg, true);
        }
        else if (old_nick == "*")
        {
            client->set_write_buffer(RPL_WELCOME(client->get_nickname()));
        }
    }
}

void Core::cmd_user(Client *client, mssg& msg)
{
    bool already_auth = client->get_is_auth();
    if (!client->get_has_password()) {
        client->set_write_buffer("464 :Please provide the server password first (PASS <password>)\r\n");
        return;
    }
    if (client->get_is_auth()) 
    {
        client->set_write_buffer(ERR_ALREADYREGISTRED(client->get_nickname()));
        return;
    }

    if (msg.args.size() < 4) 
    {
        client->set_write_buffer(ERR_NEEDMOREPARAMS(client->get_nickname(), "USER"));
        return;
    }
    client->set_username(msg.args[0]);
    client->set_realname(msg.args[3]);

    if (client->get_is_auth() && !already_auth) 
    {
        client->set_write_buffer(RPL_WELCOME(client->get_nickname()));
        std::cout << GREEN << "Client fd: " << client->get_fd() << " is fully registered!" << RESET << std::endl;
    }
}

void Core::cmd_pong(Client* client, mssg& msg)
{
    client->update_last_activity();
}

void Core::cmd_ping(Client* client, mssg& msg)
{
    if (msg.args.empty())
    {
        // 409 ERR_NOORIGIN
        client->set_write_buffer("409 " + client->get_nickname() + " :No origin specified\r\n");
        return;
    }
    client->set_write_buffer("PONG :" + msg.args[0] + "\r\n");
    std::cout << "[Core] PING received from " << client->get_nickname() << ". Sent PONG." << std::endl;
}


std::vector<std::string> Core::split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token = "";
    for (size_t i = 0; i < s.length(); ++i)
    {
        if (s[i] == delimiter)
        {
            if (!token.empty())
                tokens.push_back(token);
            token.clear();
        }
        else
        {
            token += s[i];
        }
    }
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}



bool Core::is_valid_channel_name(const std::string& name)
{

    if (name.empty() || name.length() > 50 || (name[0] != '#' && name[0] != '&'))
        return false;
        
    for (size_t i = 0; i < name.length(); ++i)
    {
        if (name[i] == ' ' || name[i] == ',')
            return false;
    }
    
    return true;
}


Client * Core::get_client(int fd)
{
std::map<int, Client*>::iterator it = clients.find(fd);
    if (it != clients.end())
        return it->second;
    return NULL;
}
