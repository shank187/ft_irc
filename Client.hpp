#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
    int         fd;
    std::string buffer;
    std::string nickname;
    std::string username;
    bool        is_registered;
    bool        is_auth;
public:

    Client();
    Client(int client_fd);
    Client(const Client& src);
    Client& operator=(const Client& rhs);
    ~Client();


    int         get_fd() const;
    std::string get_buffer() const;
    std::string get_nickname() const;
    std::string get_username() const;
    bool        get_is_registered() const;
    bool        get_is_auth() const;

    void        append_buffer(const std::string& data);
    void        extract_buffer(size_t pos);

    void        set_nickname(std::string nick);
    void        set_username(std::string user);
    void        set_auth(bool status);
    
    void        check_registration();
    void        reply(std::string msg);
};

#endif