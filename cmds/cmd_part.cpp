#include "../Core.hpp"

void Core::cmd_part(Client* client, mssg& msg)
{
    if (msg.args.size() < 1)
    {
        client->set_write_buffer(":localhost 461 " + client->get_nickname() + " PART :Not enough parameters\r\n");
        return ;
    }
    
    std::vector<std::string> channels_to_leave = split(msg.args[0], ',');
    std::string reason = client->get_nickname();
    
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
                std::string part_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " PART " + target + " :" + reason + "\r\n";
                if (part_msg.length() > 512)
                    continue;
                it->second->broadcast(part_msg, client);
                client->set_write_buffer(part_msg);
                
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
                client->set_write_buffer(":localhost 442 " + client->get_nickname() + " " + target + " :You're not on that channel\r\n");
        } 
        else
            client->set_write_buffer(":localhost 403 " + client->get_nickname() + " " + target + " :No such channel\r\n");
    }
}