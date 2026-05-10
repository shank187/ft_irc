#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
    int         _fd;                // The socket file descriptor
    std::string _buffer;            // The "pocket" for incomplete TCP packets
    std::string _nickname;          // The user's chosen IRC name
    std::string _username;          // The user's system name
    bool        _isAuthenticated;   // Has the user sent the correct PASS?

public:
    Client(); // (Required so we can put it in a std::map)
    Client(int fd);
    ~Client();


    int         getFd() const;
    std::string getBuffer() const;
    std::string getNickname() const;
    std::string getUsername() const;
    bool        isAuthenticated() const;

    void        appendBuffer(std::string const &data);
    void        extractBuffer(size_t pos); // To slice off completed commands
    void        setNickname(std::string const &nickname);
    void        setUsername(std::string const &username);
    void        setAuthenticated(bool status);
};

#endif