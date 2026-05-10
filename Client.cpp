#include "Client.hpp"

Client::Client() : _fd(-1), _buffer(""), _nickname(""), _username(""), _isAuthenticated(false) {}

Client::Client(int fd) : _fd(fd), _buffer(""), _nickname(""), _username(""), _isAuthenticated(false) {}

Client::~Client() {}

int Client::getFd() const { return _fd; }
std::string Client::getBuffer() const { return _buffer; }
std::string Client::getNickname() const { return _nickname; }
std::string Client::getUsername() const { return _username; }
bool Client::isAuthenticated() const { return _isAuthenticated; }


// Glues incoming bytes to the end of the pocket
void Client::appendBuffer(std::string const &data) { 
    _buffer += data; 
}

// Slices off the complete command from the pocket
void Client::extractBuffer(size_t pos) { 
    _buffer.erase(0, pos); 
}

void Client::setNickname(std::string const &nickname) { 
    _nickname = nickname; 
}

void Client::setUsername(std::string const &username) { 
    _username = username; 
}

void Client::setAuthenticated(bool status) { 
    _isAuthenticated = status; 
}