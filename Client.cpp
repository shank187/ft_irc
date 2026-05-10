#include "Client.hpp"
#include <iostream>


Client::Client() : fd(-1), buffer(""), nickname(""), username(""), is_registered(false), is_auth(false) {}

Client::Client(int client_fd) : fd(client_fd), buffer(""), nickname(""), username(""), is_registered(false), is_auth(false) {}

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
        this->is_registered = rhs.is_registered;
        this->is_auth = rhs.is_auth;
    }
    return *this;
}


int Client::get_fd() const {
    return fd;
}

std::string Client::get_buffer() const {
    return buffer;
}

std::string Client::get_nickname() const {
    return nickname;
}

std::string Client::get_username() const {
    return username;
}

bool Client::get_is_registered() const {
    return is_registered;
}

bool Client::get_is_auth() const {
    return is_auth;
}


void Client::append_buffer(const std::string& data) {
    buffer += data;
}

void Client::extract_buffer(size_t pos) {
    buffer.erase(0, pos);
}


void Client::set_auth(bool status) { 
    is_auth = status; 
}

void Client::set_nickname(std::string nick) {
    nickname = nick;
    check_registration();
}

void Client::set_username(std::string user) {
    username = user;
    check_registration();
}


void Client::check_registration() {
    if (!nickname.empty() && !username.empty())
        is_registered = true;
}

void Client::reply(std::string msg) {
    // exp: send(this->fd, msg.c_str(), msg.length(), 0);
    std::cout << "[To FD " << fd << "] -> " << msg << std::endl;
}