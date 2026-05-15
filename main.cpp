#include "Server.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    // 1. Check for the correct number of arguments
    if (argc != 3) {
        std::cerr<< RED << "Usage: ./ircserv <port> <password>" << RESET << std::endl;
        return 1;
    }

    // 2. Parse the port
    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    // 3. Instantiate your Server object
    Server ircServer(port, password);

    try {
        // 4. Start the engine!
        ircServer.init();
        ircServer.run();
    } 
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}