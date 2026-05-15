#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

Channel::Channel() : name(""), topic(""), mode_i(false), mode_t(false), password(""), limit(-1) {}

Channel::Channel(std::string ch_name) : name(ch_name), topic(""), mode_i(false), mode_t(false), password(""), limit(-1) {}

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

void    Channel::remove_operator(Client* client)
{
     std::vector<Client*>::iterator it;
    for (it = operators.begin(); it != operators.end(); ++it)
    {
        if (*it == client)
        {
            operators.erase(it);
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


void Channel::add_invite(Client* client)
{
    if (!is_invited(client)) {
        invited_clients.push_back(client);
    }
}

bool Channel::is_invited(Client* client) const
{
    return std::find(invited_clients.begin(), invited_clients.end(), client) != invited_clients.end();
}


void Channel::set_mode_i(bool val){ mode_i = val; }

void Channel::set_mode_t(bool val) { mode_t = val; }

void Channel::set_password(std::string pass) { password = pass; }

void Channel::set_limit(int l) { limit = l; }

bool Channel::is_invite_only() const { return mode_i; }
std::string Channel::get_password() const { return password; }
int Channel::get_limit() const { return limit; }


std::string Channel::get_modes() const
{
    std::string modes = "+";
    if (mode_i)
        modes += "i";
    if (mode_t)
        modes += "t";
    if (!password.empty())
        modes += "k";
    if (limit != -1)
        modes += "l";
    return modes;
}


bool Channel::is_topic_restricted() const
{
    return mode_t;
}

const std::vector<Client*>& Channel::get_members() const
{
    return members;
}