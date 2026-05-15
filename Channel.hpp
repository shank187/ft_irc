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

    std::vector<Client*> invited_clients;

    bool        mode_i;
    bool        mode_t;
    std::string password;
    int         limit;

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
    void        remove_operator(Client* client);
    bool        is_member(Client* client) const;
    bool        is_operator(Client* client) const;
    
    void        broadcast(std::string msg, Client* sender);


    void add_invite(Client* client);
    bool is_invited(Client* client) const;



    void        set_mode_i(bool val);
    void        set_mode_t(bool val);
    void        set_password(std::string pass);
    void        set_limit(int l);

    std::string get_modes() const;
    std::string get_password() const;
    int         get_limit() const;
    bool        is_invite_only() const;

    bool        is_topic_restricted() const;
    const       std::vector<Client*>& get_members() const;
};

#endif