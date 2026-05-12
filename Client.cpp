#include "Client.hpp"
#include <iostream>

Client::Client() : fd(-1), buffer(""), nickname(""), username(""), 
                _hasNickname(false), _hasUsername(false), _hasPassword(false) {}

Client::Client(int client_fd) : fd(client_fd), buffer(""), nickname(""), username(""), 
                                _hasNickname(false), _hasUsername(false), _hasPassword(false) {}

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

bool Client::get_is_auth() const {
    return (_hasPassword && _hasNickname && _hasUsername);
}

bool Client::get_is_registered() const {
    return get_is_auth(); 
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

void Client::reply(std::string msg) {
    // exp: send(this->fd, msg.c_str(), msg.length(), 0);
    std::cout << "[To FD " << fd << "] -> " << msg << std::endl;
}