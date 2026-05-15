#include "../Core.hpp"

// Exemple 1 (Bla reason):  "PART #1337\r\n"
// Exemple 2 (B reason):    "PART #1337 :Mcha 3liya l'7al ghan-mchi n3es\r\n"
// Exemple 3 (Multiples):   "PART #1337,#42 :Bye bye\r\n"

void Core::cmd_part(Client* client, mssg& msg)
{
    if (msg.args.size() < 1)
    {
        client->reply("461 " + client->get_nickname() + " PART :Not enough parameters\r\n");
        return ;
    }
    
    std::vector<std::string> channels_to_leave = split(msg.args[0], ',');
    std::string reason = "Leaving";
    
    if (msg.args.size() > 1)
        reason = msg.args[1];

    for (size_t i = 0; i < channels_to_leave.size(); ++i)
    {
        std::string target = channels_to_leave[i];
        std::map<std::string, Channel*>::iterator it = channels.find(target);
        
        if (it != channels.end())
        {
            if(it->second->is_member(client))
            {
                std::string part_msg = ":" + client->get_nickname() + " PART " + target + " :" + reason + "\r\n";
                it->second->broadcast(part_msg, client);
                client->reply(part_msg);
                
                it->second->remove_client(client);
                if(it->second->is_operator(client))
                    it->second->remove_operator(client);
                
                if (it->second->get_members().empty())
                {
                    delete it->second;
                    channels.erase(it);
                }
            }
            else
                client->reply("442 " + client->get_nickname() + " " + target + " :You're not on that channel\r\n");
        } 
        else
            client->reply("403 " + client->get_nickname() + " " + target + " :No such channel\r\n");
    }
}