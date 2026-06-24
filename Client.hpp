#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>


class Client {
private:
    int         fd;
    std::string buffer;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string _hostname;
    bool _hasPassword;
    bool _hasNickname;
    bool _hasUsername;
    std::string _write_buffer;

    time_t _last_activity;
    bool _waiting_for_pong;
    bool _has_quit;
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
    bool        get_has_quit() const;


    std::string get_hostname() const;
    void        set_hostname(const std::string& hostname);

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
    void        set_has_quit(bool);

    time_t get_last_activity() const;
    void update_last_activity();
    bool is_waiting_for_pong() const;
    void set_waiting_for_pong(bool val);
    
};

#endif