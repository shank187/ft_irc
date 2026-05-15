#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
    int         fd;
    std::string buffer;
    std::string nickname;
    std::string username;
    std::string realname;
    bool _hasPassword;
    bool _hasNickname;
    bool _hasUsername;
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
    bool        get_is_auth() const;

    bool        get_has_password() const;
    bool        get_has_nickname() const;
    bool        get_has_username() const;
    void        append_buffer(const std::string& data);
    void        extract_buffer(size_t pos);

    void        set_nickname(std::string nick);
    void        set_username(std::string user);
    void        set_has_password(bool status);
    void        set_realname(std::string r_name);
    void        reply(std::string msg);
};

#endif