#include "../Core.hpp"

// Exemple 1 (L'Channel):   "PRIVMSG #1337 :Salam a drari dyal l'bit!\r\n"
// Exemple 2 (L'User):      "PRIVMSG omar :Nadi a sat fin wselti?\r\n"


void Core::cmd_privmsg(Client* client, mssg& msg)
{
    if (msg.args.empty())
    {
        client->reply("411 " + client->get_nickname() + " :No recipient given (PRIVMSG)\r\n");
        return;
    }
    
    if (msg.args.size() < 2 || msg.args[1].empty())
    {
        client->reply("412 " + client->get_nickname() + " :No text to send\r\n");
        return;
    }

    std::vector<std::string> targets = split(msg.args[0], ',');
    std::string text = msg.args[1];


    if (targets.size() > 10) 
    {
        client->reply("407 " + client->get_nickname() + " " + msg.args[0] + " :Too many recipients.\r\n");
        return;
    }

    for (size_t i = 0; i < targets.size(); ++i)
    {
        std::string target = targets[i];

        if (target[0] == '#' || target[0] == '&')
        {
            std::map<std::string, Channel*>::iterator it = channels.find(target);
            
            if (it != channels.end())
            {
                if (!it->second->is_member(client))
                {
                    client->reply("404 " + client->get_nickname() + " " + target + " :Cannot send to channel\r\n");
                    continue;
                }

                std::string full_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@127.0.0.1 PRIVMSG " + target + " :" + text + "\r\n";
                it->second->broadcast(full_msg, client);
            } 
            else
                client->reply("401 " + client->get_nickname() + " " + target + " :No such nick/channel\r\n");
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
                std::string full_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@127.0.0.1 PRIVMSG " + target + " :" + text + "\r\n";
                target_client->reply(full_msg);
            }
            else
                client->reply("401 " + client->get_nickname() + " " + target + " :No such nick/channel\r\n");
        }
    }
}