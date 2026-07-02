#ifndef NUMERIC_HPP
#define NUMERIC_HPP

#include <string>

// 431
inline std::string ERR_NONICKNAMEGIVEN(std::string client) {
    return ":localhost 431 " + client + " :No nickname given\r\n";
}
// 432
inline std::string ERR_ERRONEUSNICKNAME(std::string client, std::string nick) {
    return ":localhost 432 " + client + " " + nick + " :Erroneous nickname\r\n";
}
// 433
inline std::string ERR_NICKNAMEINUSE(std::string client, std::string nick) {
    return ":localhost 433 " + client + " " + nick + " :Nickname is already in use\r\n";
}
// 461
inline std::string ERR_NEEDMOREPARAMS(std::string client, std::string command) {
    return ":localhost 461 " + client + " " + command + " :Not enough parameters\r\n";
}
// 462
inline std::string ERR_ALREADYREGISTRED(std::string client) {
    return ":localhost 462 " + client + " :Unauthorized command (already registered)\r\n";
}
// 464
inline std::string ERR_PASSWDMISMATCH(std::string client) {
    return ":localhost 464 " + client + " :Password incorrect\r\n";
}
// 001
inline std::string RPL_WELCOME(std::string client) {
    return ":localhost 001 " + client + " :Welcome to the ft_irc Network, " + client + "!\r\n";
}

#endif