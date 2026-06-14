#include "../Core.hpp"


// Exemple 1 (Chouf modes): "MODE #1337\r\n"
// Exemple 2 (Zid mode):    "MODE #1337 +i\r\n"
// Exemple 3 (7iyd mode):   "MODE #1337 -m\r\n"
// Exemple 4 (Combo args):  "MODE #1337 +ok-l ali my_password\r\n"

void Core::cmd_mode(Client* client, mssg& msg)
{
    if (msg.args.empty())
    {
        client->set_write_buffer("461 " + client->get_nickname() + " MODE :Not enough parameters\r\n");
        return ;
    }

    std::string target_channel = msg.args[0];

    if (target_channel[0] != '#' && target_channel[0] != '&') 
    {
        if (target_channel == client->get_nickname())
        {
            client->set_write_buffer("221 " + client->get_nickname() + " +i\r\n"); 
            return; 
        } 
        else {
            client->set_write_buffer("502 " + client->get_nickname() + " :Cant change mode for other users\r\n");
            return;
        }
    }
    std::map<std::string, Channel*>::iterator it = channels.find(target_channel);

    if (it != channels.end())
    {
        if (msg.args.size() == 1)//read mode in this chanel
        {
            std::string current_modes = it->second->get_modes();
            client->set_write_buffer("324 " + client->get_nickname() + " " + target_channel + " " + current_modes + "\r\n");
            return ;
        }
        if (msg.args.size() >= 2 && (msg.args[1] == "b" || msg.args[1] == "+b")) 
        {
            client->set_write_buffer("368 " + client->get_nickname() + " " + target_channel + " :End of channel ban list\r\n");
            return;
        }
        //set mode!!!!!!!!!!1
        
        if (!it->second->is_operator(client))
        {
            client->set_write_buffer("482 " + client->get_nickname() + " " + target_channel + " :You're not channel operator\r\n");
            return ;
        }


        std::string mode_str = msg.args[1];
        bool add = true;
        size_t arg_idx = 2;
        
        std::string applied_modes = "";
        std::string applied_args = "";

        for (size_t i = 0; i < mode_str.length(); i++)
        {
            char c = mode_str[i];
            
            if (c == '+')
            {
                add = true;
                applied_modes += "+";
            }
            else if (c == '-')
            {
                add = false;
                applied_modes += "-";
            }
            else if (c == 'i')
            {
                it->second->set_mode_i(add);
                applied_modes += "i";
            }
            else if (c == 't')
            {
                it->second->set_mode_t(add);
                applied_modes += "t";
            }
           else if (c == 'k')
            {
                if (add && arg_idx < msg.args.size())
                {

                    if (!it->second->get_password().empty()) {
                        client->set_write_buffer("467 " + client->get_nickname() + " " + target_channel + " :Channel key already set\r\n");
                    } else {
                        it->second->set_password(msg.args[arg_idx]);
                        applied_modes += "k";
                        applied_args += " " + msg.args[arg_idx];
                    }
                    arg_idx++;
                }
                else if (!add)
                {
                    it->second->set_password("");
                    applied_modes += "k";
                    if (arg_idx < msg.args.size())
                    {
                        applied_args += " " + msg.args[arg_idx];
                        arg_idx++;
                    }
                }
            }
            else if (c == 'l')
            {
                if (add && arg_idx < msg.args.size())
                {
                    int limit = std::atoi(msg.args[arg_idx].c_str());
                    it->second->set_limit(limit);
                    applied_modes += "l";
                    applied_args += " " + msg.args[arg_idx];
                    arg_idx++;
                }
                else if (!add)
                {
                    it->second->set_limit(-1);
                    applied_modes += "l";
                }
            }
            else if (c == 'o')
            {
                if (arg_idx < msg.args.size())
                {
                    std::string target_nick = msg.args[arg_idx];
                    arg_idx++;

                    Client* target_client = NULL;
                    std::map<int, Client*>::iterator it_c;
                    for (it_c = clients.begin(); it_c != clients.end(); it_c++)
                    {
                        if (it_c->second->get_nickname() == target_nick)
                        {
                            target_client = it_c->second;
                            break;
                        }
                    }

                    if (!target_client)
                        client->set_write_buffer("401 " + client->get_nickname() + " " + target_nick + " :No such nick/channel\r\n");
                    else if (!it->second->is_member(target_client))
                        client->set_write_buffer("441 " + client->get_nickname() + " " + target_nick + " " + target_channel + " :They aren't on that channel\r\n");
                    else
                    {
                        if (add)
                            it->second->add_operator(target_client);
                        else
                            it->second->remove_operator(target_client);
                        
                        applied_modes += "o";
                        applied_args += " " + target_nick;
                    }
                }
            }
            else
                client->set_write_buffer("472 " + client->get_nickname() + " " + c + " :is unknown mode char to me\r\n");
        }

        if (!applied_modes.empty() && applied_modes != "+" && applied_modes != "-")
        {
            std::string mode_msg = ":" + client->get_nickname() + "!" + client->get_username() + "@127.0.0.1 MODE " + target_channel + " " + applied_modes + applied_args + "\r\n";
            it->second->broadcast(mode_msg, NULL);
        }
    } 
    else
    {
        client->set_write_buffer("403 " + client->get_nickname() + " " + target_channel + " :No such channel\r\n");
    }
}