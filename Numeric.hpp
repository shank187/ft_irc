#ifndef NUMERIC_HPP
#define NUMERIC_HPP

#include <string>

// 401
inline std::string ERR_NOSUCHNICK(std::string client, std::string target) {
    return "401 " + client + " " + target + " :No such nick/channel\r\n";
}
// 412
inline std::string ERR_NOTEXTTOSEND(std::string client) {
    return "412 " + client + " :No text to send\r\n";
}
// 431
inline std::string ERR_NONICKNAMEGIVEN(std::string client) {
    return "431 " + client + " :No nickname given\r\n";
}
// 432
inline std::string ERR_ERRONEUSNICKNAME(std::string client, std::string nick) {
    return "432 " + client + " " + nick + " :Erroneous nickname\r\n";
}
// 433
inline std::string ERR_NICKNAMEINUSE(std::string client, std::string nick) {
    return "433 " + client + " " + nick + " :Nickname is already in use\r\n";
}
// 461
inline std::string ERR_NEEDMOREPARAMS(std::string client, std::string command) {
    return "461 " + client + " " + command + " :Not enough parameters\r\n";
}
// 462
inline std::string ERR_ALREADYREGISTRED(std::string client) {
    return "462 " + client + " :Unauthorized command (already registered)\r\n";
}
// 464
inline std::string ERR_PASSWDMISMATCH(std::string client) {
    return "464 " + client + " :Password incorrect\r\n";
}
// 001
inline std::string RPL_WELCOME(std::string client) {
    return "001 " + client + " :Welcome to the ft_irc Network, " + client + "!\r\n";
}

#endif