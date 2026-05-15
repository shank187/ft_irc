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

    std::string target_channel = msg.args[0];
    std::vector<std::string> targets = split(msg.args[1], ',');
    
    std::string reason = "kicked by admin :)";
    if (msg.args.size() > 2)
        reason = msg.args[2];
    
    std::map<std::string, Channel*>::iterator it = channels.find(target_channel);

    if (it != channels.end())
    {
        if(it->second->is_member(client))
        {
            if(it->second->is_operator(client))
            {
                for (size_t i = 0; i < targets.size(); ++i)
                {
                    std::string target_nickname = targets[i];
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

                    if (it->second->is_member(target_client))
                    {
                        std::string kick_msg = ":" + client->get_nickname() + " KICK " + target_channel + " " + target_nickname + " :" + reason + "\r\n";
                        it->second->broadcast(kick_msg, NULL);

                        it->second->remove_client(target_client);
                        if (it->second->is_operator(target_client))
                            it->second->remove_operator(target_client);
                        
                        if (it->second->get_members().empty())
                        {
                            delete it->second;
                            channels.erase(it);
                            break;
                        }
                    }
                    else
                    {
                        client->reply("441 " + client->get_nickname() + " " + target_nickname + " " + target_channel + " :They aren't on that channel\r\n");
                    }
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
    } 
    else
    {
        client->reply("403 " + client->get_nickname() + " " + target_channel + " :No such channel\r\n");
    }
}