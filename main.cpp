#include "Server.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    // 1. Check for the correct number of arguments
    if (argc != 2) {
        std::cerr << "Usage: ./ircserv <port>" << std::endl;
        return 1;
    }

    // 2. Parse the port
    int port = std::atoi(argv[1]);

    // 3. Instantiate your Server object
    Server ircServer(port);

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