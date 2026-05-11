#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>


class Client;

class Channel {
private:
    std::string             name;
    std::string             topic;

    std::vector<Client*>    members;
    std::vector<Client*>    operators;

public:
    Channel();
    Channel(std::string ch_name);
    Channel(const Channel& src);
    Channel& operator=(const Channel& rhs);
    ~Channel();

    std::string get_name() const;
    std::string get_topic() const;

    void        set_topic(std::string ch_topic);

    void        add_client(Client* client);
    void        remove_client(Client* client);
    void        add_operator(Client* client);
    bool        is_member(Client* client) const;
    bool        is_operator(Client* client) const;
    
    void        broadcast(std::string msg, Client* sender);
};

#endif