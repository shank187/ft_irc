#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

Channel::Channel() : name(""), topic("") {}

Channel::Channel(std::string ch_name) : name(ch_name), topic("") {}

Channel::~Channel() {}

Channel::Channel(const Channel& src)
{
    *this = src;
}

Channel& Channel::operator=(const Channel& rhs)
{
    if (this != &rhs) {
        this->name = rhs.name;
        this->topic = rhs.topic;
        this->members = rhs.members;
        this->operators = rhs.operators;
    }
    return *this;
}

std::string Channel::get_name() const
{ 
    return name; 
}

std::string Channel::get_topic() const
{ 
    return topic; 
}

void Channel::set_topic(std::string ch_topic)
{ 
    topic = ch_topic; 
}

void Channel::add_client(Client* client)
{
    if (!is_member(client)) {
        members.push_back(client);
    }
}

void Channel::remove_client(Client* client)
{
    std::vector<Client*>::iterator it;
    for (it = members.begin(); it != members.end(); ++it)
    {
        if (*it == client)
        {
            members.erase(it);
            break;
        }
    }
}

void Channel::add_operator(Client* client)
{
    if (!is_operator(client)) {
        operators.push_back(client);
    }
}

bool Channel::is_member(Client* client) const
{
    return std::find(members.begin(), members.end(), client) != members.end();
}

bool Channel::is_operator(Client* client) const
{
    return std::find(operators.begin(), operators.end(), client) != operators.end();
}

void Channel::broadcast(std::string msg, Client* sender)
{
    for (size_t i = 0; i < members.size(); ++i)
    {
        if (members[i] != sender)
            members[i]->reply(msg);
    }
}