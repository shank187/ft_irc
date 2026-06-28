#include "../Core.hpp"

void Core::cmd_notice(Client* client, mssg& msg)
{
    if (msg.args.empty() || msg.args.size() < 2 || msg.args[1].empty())
        return;

    std::vector<std::string> targets = split(msg.args[0], ',');
    std::string text = msg.args[1];

    if (targets.size() > 10) 
        return;

    for (size_t i = 0; i < targets.size(); ++i)
    {
        std::string target = targets[i];

        if (target[0] == '#' || target[0] == '&')
        {
            std::map<std::string, Channel*>::iterator it = channels.find(target);
            
            if (it != channels.end() && it->second->is_member(client))
            {
                std::string full_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " NOTICE " + target + " :" + text + "\r\n";
                if (full_msg.length() > 512)
                    continue;
                it->second->broadcast(full_msg, client);
            } 
        }
        else 
        {
            Client* target_client = NULL;
            std::map<int, Client*>::iterator it;
            
            for (it = clients.begin(); it != clients.end(); ++it)
            {
                if (it->second->get_nickname() == target)
                {
                    target_client = it->second;
                    break;
                }
            }

            if (target_client)
            {
                std::string full_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " NOTICE " + target + " :" + text + "\r\n";
                if (full_msg.length() > 512)
                    continue;
                target_client->set_write_buffer(full_msg);
            }
        }
    }
}