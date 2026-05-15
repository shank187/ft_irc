#include "../Core.hpp"

// Exemple 1 (L'Channel):   "PRIVMSG #1337 :Salam a drari dyal l'bit!\r\n"
// Exemple 2 (L'User):      "PRIVMSG omar :Nadi a sat fin wselti?\r\n"


void Core::cmd_privmsg(Client* client, mssg& msg)
{

    if (msg.args.size() < 2)
    {
        client->reply("412 " + client->get_nickname() + " :No text to send\r\n");
        return;
    }

    std::string target = msg.args[0];
    std::string text = msg.args[1];
    if (target[0] == '#')//channel
    {
        std::map<std::string, Channel*>::iterator it = channels.find(target);
        
        if (it != channels.end())
        {
            std::string full_msg = ":" + client->get_nickname() + " PRIVMSG " + target + " :" + text + "\r\n";
            it->second->broadcast(full_msg, client);
        } else
            client->reply("401 " + client->get_nickname() + " " + target + " :No such nick/channel\r\n");
    }
    else //prv_client
    {
        Client* target_client = NULL;
        std::map<int, Client*>::iterator it;
        
        for (it = clients.begin(); it != clients.end(); it++)
        {
            if (it->second->get_nickname() == target)
            {
                target_client = it->second;
                break;
            }
        }

        if (target_client)
        {
            std::string full_msg = ":" + client->get_nickname() + " PRIVMSG " + target + " :" + text + "\r\n";
            target_client->reply(full_msg);
        }
        else
            client->reply("401 " + client->get_nickname() + " " + target + " :No such nick/channel\r\n");
    }

}