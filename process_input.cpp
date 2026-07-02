#include "Core.hpp"
#include <iostream>

mssg Core::parse_msg(std::string& line)
{
    mssg msg;

    if (!line.empty() && line[line.size() - 1] == '\n')
        line.erase(line.size() - 1);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    
    size_t i = 0;
    while (i < line.size() && line[i] == ' ') i++;

    if(i < line.size() && line[i] == ':') {
        size_t sp_index = line.find(' ', i);
        if (sp_index != std::string::npos) {
            msg.sender = line.substr(i + 1, sp_index - i - 1);
            i = sp_index;
        }
    }

    while (i < line.size() && line[i] == ' ') i++;

    size_t sp_index = line.find(' ', i);
    if (sp_index != std::string::npos) {
        msg.cmd = line.substr(i, sp_index - i);
        i = sp_index;
    } else {
        if (i < line.size()) msg.cmd = line.substr(i);
        return msg;
    }

    while (i < line.size()) {
        while (i < line.size() && line[i] == ' ') i++;
        if (i >= line.size()) break;
            
        if (line[i] == ':') {
            msg.args.push_back(line.substr(i+1));
            if(msg.cmd == "TOPIC" && msg.args.size() >= 2 && msg.args[1] == "")
                msg.args[1] = ":";
            break;
        } else {
            size_t sp_index = line.find(' ', i);
            if (sp_index != std::string::npos) {
                msg.args.push_back(line.substr(i, sp_index - i));
                i = sp_index;
            } else {
                msg.args.push_back(line.substr(i));
                break;
            }
        }
    }
    return msg;
}


bool Core::process_input(int fd, std::string& text)
{

    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it == clients.end())
        return true;


    Client* client = it->second;

    mssg parsed = parse_msg(text);

    if (parsed.cmd == "QUIT")
    {
        cmd_quit(client, parsed);
        return true;
    }
    if(parsed.cmd == "CAP" || parsed.cmd == "WHOIS" || parsed.cmd == "WHO") return true;
    if(parsed.cmd == "PASS")
        cmd_pass(client, parsed);
    else if(parsed.cmd == "NICK")
        cmd_nick(client, parsed);
    else if(parsed.cmd == "USER")
        cmd_user(client, parsed);
    else if (client->get_is_auth()){ 
        if (parsed.cmd == "JOIN")
            cmd_join(client, parsed);
        else if (parsed.cmd == "PRIVMSG")
            cmd_privmsg(client, parsed);
        else if (parsed.cmd == "PART")
            cmd_part(client, parsed);
        else if (parsed.cmd == "KICK")
            cmd_kick(client, parsed);
        else if (parsed.cmd == "INVITE")
            cmd_invite(client, parsed);
        else if (parsed.cmd == "TOPIC")
            cmd_topic(client, parsed);
        else if (parsed.cmd == "MODE")
            cmd_mode(client, parsed);
        else if (parsed.cmd == "PING")
            cmd_ping(client, parsed);
        else if (parsed.cmd == "PONG")
            cmd_pong(client, parsed);
        else if (!parsed.cmd.empty())
            client->set_write_buffer(":localhost 421 " + client->get_nickname() + " " + parsed.cmd + " :Unknown command\r\n");
    }
    else if(!parsed.cmd.empty() && parsed.cmd != "PASS" && parsed.cmd != "NICK" && parsed.cmd != "USER")
    {
        std::cout << YELLOW << "ignored cmd: " << RESET << parsed.cmd << std::endl;

        std::string target;
        if (client->get_nickname().empty())
            target = "*";
        else
            target = client->get_nickname();

        client->set_write_buffer(":localhost 451 " + target + " :You have not registered\r\n");
    }
    return true; 
}