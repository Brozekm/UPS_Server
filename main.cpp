#include "server/MyServer.h"





int main(int argc, char** argv) {
    if(argc == 3){
        int port;
        try {
            port = std::stoi(argv[2]);
        } catch (const std::exception& exception) {
            std::cout << "Server port is not number, correct port e.g.: 10001" << std::endl;
            return 0;
        }
        std::string address = argv[1];
        MyServer server(address,port);
        server.runServer();
    } else{
        std::cout << "Wrong number of parameters to run the server, correct format: <server_address> <server_port>" << std::endl;
    }
    return 0;
}

