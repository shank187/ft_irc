#include "../Core.hpp"

//////////////////////////////////args ->

// Exemple 1 (Bla pass):    "JOIN #1337\r\n"
// Exemple 2 (B pass):      "JOIN #1337 secret_pass\r\n"
// Exemple 3 (Multiples):   "JOIN #1337,#42 pass1,pass2\r\n"

void Core::cmd_join(Client* client, mssg& msg) {
    if (msg.args.empty())
    {
        client->reply("461 " + client->get_nickname() + " JOIN :Not enough parameters\r\n");
        return;
    }


    std::vector<std::string> channels_to_join = split(msg.args[0], ',');
    
    std::vector<std::string> keys;
    if (msg.args.size() > 1)
        keys = split(msg.args[1], ',');

    for (size_t i = 0; i < channels_to_join.size(); i++)
    {
        std::string chan_name = channels_to_join[i];
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

            if (channel->is_invite_only() && !channel->is_invited(client))
            {
                client->reply("473 " + client->get_nickname() + " " + chan_name + " :Cannot join channel (+i)\r\n");
                continue;
            }

            if (!channel->get_password().empty())
            {
                std::string provided_pass = "";
                if (i < keys.size())
                    provided_pass = keys[i];
            
                if (provided_pass != channel->get_password())
                {
                    client->reply("475 " + client->get_nickname() + " " + chan_name + " :Cannot join channel (+k)\r\n");
                    continue;
                }
            }

            if (channel->get_limit() != -1 && static_cast<int>(channel->get_members().size()) >= channel->get_limit())
            {
                client->reply("471 " + client->get_nickname() + " " + chan_name + " :Cannot join channel (+l)\r\n");
                continue;
            }
        }

        channel->add_client(client);
    
        std::string join_msg = ":" + client->get_nickname() + " JOIN :" + chan_name + "\r\n";
        channel->broadcast(join_msg, NULL);


        //chanel list numbers !!!!!!!!!!11
        std::string names_list = "";
        const std::vector<Client*>& members = channel->get_members();
        
        for (size_t j = 0; j < members.size(); j++)
        {
            if (channel->is_operator(members[j]))
                names_list += "@";
            names_list += members[j]->get_nickname();
            if (j != members.size() - 1)
                names_list += " ";
        }
        client->reply("353 " + client->get_nickname() + " = " + chan_name + " :" + names_list + "\r\n");
        client->reply("366 " + client->get_nickname() + " " + chan_name + " :End of /NAMES list\r\n");
    }
}