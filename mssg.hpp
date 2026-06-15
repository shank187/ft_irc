#ifndef MSSG_HPP
#define MSSG_HPP


#include <string>
#include <vector>

class mssg
{
    public:    
        std::string                 sender;
        std::string                 cmd;
        std::vector<std::string>    args;        
        mssg();
        ~mssg();
};




// mssg parse_msg(std::string line);

#endif