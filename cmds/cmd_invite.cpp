#include "../Core.hpp"


// Exemple (Wa7d li kayn):  "INVITE ali #1337\r\n"

void Core::cmd_invite(Client* client, mssg& msg)
{
    if (msg.args.size() < 2)
    {
        client->reply("461 " + client->get_nickname() + " INVITE :Not enough parameters\r\n");
        return ;
    }

    std::string target_nickname = msg.args[0];
    std::string target_channel = msg.args[1];
    Client* target_client = NULL;
    std::map<int, Client*>::iterator it_c;
    for (it_c = clients.begin(); it_c != clients.end(); ++it_c)
    {
        if (it_c->second->get_nickname() == target_nickname)
        {
            target_client = it_c->second;
            break;
        }
    }
    if (target_client == NULL)
    {
        client->reply("401 " + client->get_nickname() + " " + target_nickname + " :No such nick/channel\r\n");
        return ;
    }

    std::map<std::string, Channel*>::iterator it = channels.find(target_channel);
    if (it != channels.end())
    {
        if(it->second->is_member(client))
        {
            bool can_invite = true;
            
            if (it->second->is_invite_only() && !it->second->is_operator(client))
                can_invite = false;

            if(can_invite)
            {
                if(it->second->is_member(target_client))
                    client->reply("443 " + client->get_nickname() + " " + target_nickname + " " + target_channel + " :is already on channel\r\n");
                else
                {
                    it->second->add_invite(target_client);
                    
                    client->reply("341 " + client->get_nickname() + " " + target_nickname + " " + target_channel + "\r\n");
                    
                    std::string invite_msg = ":" + client->get_nickname() + " INVITE " + target_nickname + " :" + target_channel + "\r\n";
                    target_client->reply(invite_msg);
                }
            }
            else
            {
                client->reply("482 " + client->get_nickname() + " " + target_channel + " :You're not channel operator\r\n");
            }
        }
        else
        {
            client->reply("442 " + client->get_nickname() + " " + target_channel + " :You're not on that channel\r\n");
        }
    } else
    {
        client->reply("403 " + client->get_nickname() + " " + target_channel + " :No such channel\r\n");
    }
}