#include "../Core.hpp"

void Core::cmd_join(Client* client, mssg& msg) {
    if (msg.args.empty())
    {
        client->set_write_buffer(":localhost 461 " + client->get_nickname() + " JOIN :Not enough parameters\r\n");
        return;
    }

    if (msg.args[0] == "0")
    {
        std::map<std::string, Channel*>::iterator it = channels.begin();
        while (it != channels.end())
        {
            if (it->second->is_member(client))
            {
                std::string part_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " PART " + it->first + " :Left all channels\r\n";
                it->second->broadcast(part_msg, NULL);

                it->second->remove_client(client);
                if (it->second->is_operator(client))
                    it->second->remove_operator(client);

                if (it->second->get_members().empty())
                {
                    delete it->second;
                    channels.erase(it++);
                }
                else
                {
                    it++;
                }
            }
            else
            {
                it++;
            }
        }
        return;
    }

    std::vector<std::string> channels_to_join = split(msg.args[0], ',');
    
    std::vector<std::string> keys;
    if (msg.args.size() > 1)
        keys = split(msg.args[1], ',');

    for (size_t i = 0; i < channels_to_join.size(); i++)
    {
        std::string chan_name = channels_to_join[i];
        if (!is_valid_channel_name(chan_name))
        {
            client->set_write_buffer(":localhost 403 " + client->get_nickname() + " " + chan_name + " :No such channel\r\n");
            continue;
        }

        int current_channels = 0;
        std::map<std::string, Channel*>::iterator c_it;
        for (c_it = channels.begin(); c_it != channels.end(); ++c_it)
        {
            if (c_it->second->is_member(client))
                current_channels++;
        }
        if (current_channels >= 10)
        {
            client->set_write_buffer(":localhost 405 " + client->get_nickname() + " " + chan_name + " :You have joined too many channels\r\n");
            continue;
        }

        std::map<std::string, Channel*>::iterator it = channels.find(chan_name);
        Channel* channel;

        if (it == channels.end())
        {
            channel = new Channel(chan_name);
            channels[chan_name] = channel;
            channel->add_operator(client);
        }
        else
        {
            channel = it->second;
            if (channel->is_member(client))
                continue;

            if (channel->is_invite_only() && !channel->is_invited(client))
            {
                client->set_write_buffer(":localhost 473 " + client->get_nickname() + " " + chan_name + " :Cannot join channel (+i)\r\n");
                continue;
            }

            if (!channel->get_password().empty())
            {
                std::string provided_pass = "";
                if (i < keys.size())
                    provided_pass = keys[i];
            
                if (provided_pass != channel->get_password())
                {
                    client->set_write_buffer(":localhost 475 " + client->get_nickname() + " " + chan_name + " :Cannot join channel (+k)\r\n");
                    continue;
                }
            }

            if (channel->get_limit() != -1 && static_cast<int>(channel->get_members().size()) >= channel->get_limit())
            {
                client->set_write_buffer(":localhost 471 " + client->get_nickname() + " " + chan_name + " :Cannot join channel (+l)\r\n");
                continue;
            }
        }

        channel->add_client(client);
        
        if (channel->is_invited(client))
            channel->remove_invite(client);
    
        std::string join_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " JOIN :" + chan_name + "\r\n";
        channel->broadcast(join_msg, NULL);

        if (!channel->get_topic().empty())
            client->set_write_buffer(":localhost 332 " + client->get_nickname() + " " + chan_name + " :" + channel->get_topic() + "\r\n");
        else
            client->set_write_buffer(":localhost 331 " + client->get_nickname() + " " + chan_name + " :No topic is set\r\n");


        //chanel list numbers !!!!!!!!!!11
        std::string base_msg = ":localhost 353 " + client->get_nickname() + " = " + chan_name + " :";
        std::string names_list = "";
        const std::vector<Client*>& members = channel->get_members();
        
        for (size_t j = 0; j < members.size(); j++)
        {
            std::string current_nick = "";
            if (channel->is_operator(members[j]))
                current_nick += "@";
            current_nick += members[j]->get_nickname();

            if (base_msg.length() + names_list.length() + current_nick.length() + 1 > 510)
            {
                client->set_write_buffer(base_msg + names_list + "\r\n");
                names_list = current_nick;
            }
            else
            {
                if (!names_list.empty())
                    names_list += " ";
                names_list += current_nick;
            }
        }
         
        if (!names_list.empty())
        {
            client->set_write_buffer(base_msg + names_list + "\r\n");
        }
        
        client->set_write_buffer(":localhost 366 " + client->get_nickname() + " " + chan_name + " :End of NAMES list\r\n");
    }
}