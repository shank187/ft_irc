#include "Client.hpp"
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <sys/socket.h> // ADD THIS FOR send()

// 1. Default Constructor
Client::Client() : fd(-1), buffer(""), nickname(""), username(""), 
                _hasPassword(false), _hasNickname(false), _hasUsername(false),
                _last_activity(time(NULL)), _waiting_for_pong(false) {} // <-- Added initializations

// 2. Parameterized Constructor
Client::Client(int client_fd) : fd(client_fd), buffer(""), nickname(""), username(""), 
                                _hasPassword(false), _hasNickname(false), _hasUsername(false),
                                _last_activity(time(NULL)), _waiting_for_pong(false) {} // <-- Added initializations
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
}

time_t Client::get_last_activity() const
{
    return _last_activity;
}

void Client::update_last_activity()
{
    _last_activity = time(NULL);
    _waiting_for_pong = false;
}

bool Client::is_waiting_for_pong() const
{
    return _waiting_for_pong;
}

void Client::set_waiting_for_pong(bool val)
{
    _waiting_for_pong = val;
}


std::string Client::get_hostname() const
{
    return this->_hostname;
}


void Client::set_hostname(const std::string& hostname)
{
    this->_hostname = hostname;
}