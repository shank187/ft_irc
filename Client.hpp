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
    std::string _write_buffer;
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
    std::string get_write_buffer();
    bool        get_is_auth() const;

    bool        get_has_password() const;
    bool        get_has_nickname() const;
    bool        get_has_username() const;
    void        append_buffer(const std::string& data);
    void        extract_buffer(size_t pos);
    void        erase_from_write_buffer(size_t bytes);

    void        set_nickname(std::string nick);
    void        set_username(std::string user);
    void        set_has_password(bool status);
    void        set_realname(std::string r_name);
    void        set_write_buffer(std::string msg);
    
};

#endif