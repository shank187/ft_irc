#include "Core.hpp"
#include <iostream>
#include <string>

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

void Core::set_password(const std::string & pw){
    _server_password = pw;
}


void Core::cmd_pass(Client *client, mssg& msg)
{
    if (client->get_has_password())
        return;
    if (msg.args.size())
    {
        if(msg.args[0] == _server_password)
        {
            client->set_has_password(true);
            client->reply("Correct! passoword check passed\r\n");
            std::cout << GREEN << "Client fd: " << client->get_fd() << ", has passed password check." << std::endl;
            if(!client->get_has_nickname())
                client->reply("add your Nickname to complete your registration! (NICK ur_nickname)\r\n");
            if(!client->get_has_username())
                client->reply("add your Usename to complete your registration! (USER ur_username)\r\n");
            else if (client->get_has_nickname())
                client->reply("you are successfully authenticated\r\n");
        }
        else{
            client->reply("Incorrect passoword! try again\r\n");
            std::cout << YELLOW << "Client fd: " << client->get_fd() << ", has entered a incorrect pw." << std::endl;
        }
    }
    else
    {
        client->reply("431 :No password given\r\n");
    }
}

bool Core::validate_nickname(const std::string &nick)
{
    if(nick.size() <= 1 || nick.size() >= 30)
        return false;
    for(size_t i = 0; i < nick.size(); i++)
    {
        if((nick[i] <= 'a' || nick[i]>='z') &&
            (nick[i] <= 'A' || nick[i] >= 'Z') &&
            (i && nick[i] <= '0' || nick[i] >= '9') &&
            (i && nick[i] != '_' || nick[i] != '\\' ||
            nick[i] != '{' || nick[i] != '}' ||
            nick[i] != '[' ||  nick[i] != ']'))
            return (false);
    }
    return (true);

}

bool Core::check_is_nick_exist(const std::string &)
{
    std::map<int, Client *>::iterator it;
    for(it = clients.begin(); it != clients.end(); it++)
    {
        Client &client = it->second;
    }
}


void Core::cmd_nick(Client *client, mssg& msg)
{
    if(msg.args.empty())
    {
        client->reply("431: No Nickname given\r\n");
        return;
    }
    if(!validate_nickname(msg.args[0]))
        client->reply("432 " + msg.args[0] + " :Erroneous nickname\r\n");
        
    

}

void Core::cmd_user(Client *client, mssg& msg)
{

}


void Core::cmd_join(Client* client, mssg& msg) {
    if (msg.args.empty())
    {
        client->reply("461 " + client->get_nickname() + " JOIN :Not enough parameters\r\n");
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
        client->reply("412 " + client->get_nickname() + " :No text to send\r\n");
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
        client->reply("401 " + client->get_nickname() + " " + target + " :No such nick/channel\r\n");
    }
}



void Core::cmd_part(Client* client, mssg& msg)
{

}

void Core::cmd_kick(Client* client, mssg& msg)
{
    
}