#include "../Core.hpp"


// Exemple 1 (Bla reason):  "KICK #1337 omar\r\n"
// Exemple 2 (B reason):    "KICK #1337 omar :Zrebti w khsserti l'hdra\r\n"


void Core::cmd_kick(Client* client, mssg& msg)
{
    if (msg.args.size() < 2)
    {
       client->reply("461 " + client->get_nickname() + " KICK :Not enough parameters\r\n");
       return ;
    }

    std::vector<std::string> target_channels = split(msg.args[0], ',');
    std::vector<std::string> targets = split(msg.args[1], ',');
    
    if (target_channels.size() != 1 && target_channels.size() != targets.size())
    {
        client->reply("461 " + client->get_nickname() + " KICK :Not enough parameters\r\n");
        return;
    }

    std::string reason = "kicked by admin :)";
    if (msg.args.size() > 2)
        reason = msg.args[2];
    
    for (size_t i = 0; i < targets.size(); ++i)
    {
        std::string current_channel_name;
        if (target_channels.size() == 1)
            current_channel_name = target_channels[0];
        else
            current_channel_name = target_channels[i];
    
        std::string target_nickname = targets[i];

        std::map<std::string, Channel*>::iterator it = channels.find(current_channel_name);
        if (it == channels.end())
        {
            client->reply("403 " + client->get_nickname() + " " + current_channel_name + " :No such channel\r\n");
            continue;
        }

        Channel* channel = it->second;

        if (!channel->is_member(client))
        {
            client->reply("442 " + client->get_nickname() + " " + current_channel_name + " :You're not on that channel\r\n");
            continue;
        }

        if (!channel->is_operator(client))
        {
            client->reply("482 " + client->get_nickname() + " " + current_channel_name + " :You're not channel operator\r\n");
            continue;
        }

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
            continue;
        }

        if (channel->is_member(target_client))
        {
            std::string kick_msg = ":" + client->get_nickname() + " KICK " + current_channel_name + " " + target_nickname + " :" + reason + "\r\n";
            channel->broadcast(kick_msg, NULL);


            channel->remove_client(target_client);
            if (channel->is_operator(target_client))
                channel->remove_operator(target_client);
            
            if (channel->get_members().empty())
            {
                delete channel;
                channels.erase(it);
            }
        }
        else
        {
            client->reply("441 " + client->get_nickname() + " " + target_nickname + " " + current_channel_name + " :They aren't on that channel\r\n");
        }
    }
}