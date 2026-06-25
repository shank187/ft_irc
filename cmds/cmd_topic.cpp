#include "../Core.hpp"
#include <iostream>

void Core::cmd_topic(Client* client, mssg& msg)
{
    if (msg.args.empty())
    {
        client->set_write_buffer(":localhost 461 " + client->get_nickname() + " TOPIC :Not enough parameters\r\n");
        return ;
    }

    std::string target_channel = msg.args[0];
    std::map<std::string, Channel*>::iterator it = channels.find(target_channel);

    if (it != channels.end())
    {
        if (it->second->is_member(client))
        {
            if (msg.args.size() == 1)
            {
                if (it->second->get_topic().empty())
                    client->set_write_buffer(":localhost 331 " + client->get_nickname() + " " + target_channel + " :No topic is set\r\n");
                else
                    client->set_write_buffer(":localhost 332 " + client->get_nickname() + " " + target_channel + " :" + it->second->get_topic() + "\r\n");
            }
            else
            {
                bool can_change = true;
            
                if (it->second->is_topic_restricted() && !it->second->is_operator(client))
                    can_change = false;

                if (can_change)
                {
                    if (msg.args[1] == ":")
                    {
                        it->second->set_topic("");

                        std::string topic_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " TOPIC " + target_channel + " :\r\n";
                        it->second->broadcast(topic_msg, NULL);
                    }
                    else
                    {
                        std::string new_topic = msg.args[1];

                        it->second->set_topic(new_topic);

                        std::string topic_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " TOPIC " + target_channel + " :" + new_topic + "\r\n";
                        it->second->broadcast(topic_msg, NULL);
                    }
                }
                else
                {
                    client->set_write_buffer(":localhost 482 " + client->get_nickname() + " " + target_channel + " :You're not channel operator\r\n");
                }
            }
        }
        else
        {
            client->set_write_buffer(":localhost 442 " + client->get_nickname() + " " + target_channel + " :You're not on that channel\r\n");
        }
    }
    else
    {
        client->set_write_buffer(":localhost 403 " + client->get_nickname() + " " + target_channel + " :No such channel\r\n");
    }
}