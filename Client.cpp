#include "Client.hpp"
#include <iostream>
#include <sys/socket.h> // ADD THIS FOR send()

// 1. Default Constructor
Client::Client() : fd(-1), buffer(""), nickname(""), username(""), 
                _hasPassword(false), _hasNickname(false), _hasUsername(false) {}

// 2. Parameterized Constructor
Client::Client(int client_fd) : fd(client_fd), buffer(""), nickname(""), username(""), 
                                _hasPassword(false), _hasNickname(false), _hasUsername(false) {}

Client::~Client() {}

Client::Client(const Client& src) {
    *this = src;
}

Client& Client::operator=(const Client& rhs)
{
    if (this != &rhs) {
        this->fd = rhs.fd;
        this->buffer = rhs.buffer;
        this->nickname = rhs.nickname;
        this->username = rhs.username;
        
        this->_hasPassword = rhs._hasPassword;
        this->_hasNickname = rhs._hasNickname;
        this->_hasUsername = rhs._hasUsername;
    }
    return *this;
}

// --- Getters ---
int Client::get_fd() const { return fd; }
std::string Client::get_buffer() const { return buffer; }
std::string Client::get_nickname() const { return nickname; }
std::string Client::get_username() const { return username; }

bool Client::get_has_password() const { return _hasPassword; }

bool Client::get_has_nickname() const { return _hasNickname; }
bool Client::get_has_username() const { return _hasUsername; }

bool Client::get_is_auth() const {
    return (_hasPassword && _hasNickname && _hasUsername);
}


// --- Setters ---
void Client::append_buffer(const std::string& data) { buffer += data; }
void Client::extract_buffer(size_t pos) { buffer.erase(0, pos); }

// Step 1 of Registration
void Client::set_has_password(bool status) { 
    _hasPassword = status; 
}

// Step 2 of Registration
void Client::set_nickname(std::string nick) {
    nickname = nick;
    _hasNickname = true;
}

// Step 3 of Registration
void Client::set_username(std::string user) {
    username = user;
    _hasUsername = true;
}

void Client::set_realname(std::string r_name)
{
    realname = r_name;
}

std::string Client::get_write_buffer()
{
    return _write_buffer;
}

void Client::erase_from_write_buffer(size_t bytes)
{
    _write_buffer.erase(0, bytes);
}

void Client::set_write_buffer(std::string msg)
{
    _write_buffer += msg;
    

    // size_t total_sent = 0;
    // size_t bytes_left = msg.length();
    // ssize_t n = 0;

    // while (total_sent < msg.length())
    // {
    //     n = send(this->fd, msg.c_str() + total_sent, bytes_left, 0);
    //     if (n == -1)
    //         break;
    //     total_sent += n;
    //     bytes_left -= n;
    // }
}