//
// Created by martin on 18.11.20.
//

#include "MyServer.h"
#include "../requests/CommunicationParser.h"
#include "../Game/Player.h"
#include "../Game/GameRPS.h"
#include <arpa/inet.h>

MyServer::MyServer(const std::string& address, int port) {
    int r_value =  init(address,port);
    if(r_value != 0){
        if (r_value ==-2){
            std::cout << "Incorrect port" << std::endl;
            std::exit(1);
        }
        perror("Server initialization failed");
        std::exit(1);
    }else{
        std::cout << "Successful server initialization" << std::endl;
    }
}

int MyServer::init(const std::string& address, int port){
    if (!correctPort(port)){
        return -2;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int enableOpt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enableOpt, sizeof(int))< 0){
        std::cout << "Address re-usability failed!" << std:: endl;
    }
    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr(address.c_str());

    return_value = bind(server_socket, (struct sockaddr *) &my_addr, \
            sizeof(struct sockaddr_in));

    if (return_value == 0)
        printf("Bind - OK\n");
    else {
        printf("Bind - ERR\n");
        return -1;
    }

    return_value = listen(server_socket, 5);
    if (return_value == 0){
        printf("Listen - OK\n");
    } else {
        printf("Listen - ER\n");
    }

    return 0;
}

int MyServer::runServer() {

    FD_ZERO( &client_socks );
    FD_SET( server_socket, &client_socks );

    while(true){

        tests = client_socks;
        return_value = select( FD_SETSIZE, &tests, ( fd_set *)0, ( fd_set *)0, ( struct timeval *)0 );

        if (return_value < 0) {
            printf("Select - ERR\n");
            return -1;
        }

        for( fd = 3; fd < FD_SETSIZE; fd++ ){

            if( FD_ISSET( fd, &tests ) ){

                if (fd == server_socket){
                    client_socket = accept(server_socket, (struct sockaddr *) &peer_addr,
                                           reinterpret_cast<socklen_t *>(&len_addr));
                    FD_SET( client_socket, &client_socks );
                    printf("New client connected and added to socket set\n");
                }

                else {

                    ioctl( fd, FIONREAD, &a2read );

                    if (a2read > 0){
                        recv(fd, &cbuf, 32, 0);
                        std::string reqStr(cbuf, cbufLength());
                        Response response(CommunicationParser::clientReqParser(fd,reqStr));
                        if(response.successful){
                            send(fd, response.response.c_str(), response.response.length(),0);
                        }else{
                            send(fd, response.response.c_str(), response.response.length(),0);
                            deleteAllSocketConnections(fd);
                            printf("Client logged out and was removed from socket set for using wrong protocol\n");
                        }

                    }

                    else {
                        clientDisconnected(fd);
//                        deleteAllSocketConnections(fd);
                        printf("Client disconnected\n");
                    }
                }
            }
        }

    }
}

int MyServer::cbufLength() {
    int length = 0;
    for(char i : cbuf){
        if (i == '\n'){
            break;
        }
        length++;
    }
    return length;
}

void MyServer::deleteAllSocketConnections(int fd) {
    for (int i=0; i<Players::allPlayers.size(); i++){
        if(fd == Players::allPlayers[i].socket){
            if(Players::allPlayers[i].state == IN_GAME){
                for(int y = 0 ; y < Games::allGames.size(); y++){
                    if(Games::allGames[y].player_1.id == Players::allPlayers[i].id){
                            std::string tmpResponse = std::to_string(WIN_SURR)+"|"+std::to_string(Games::allGames[y].score_2)+"|"+std::to_string(Games::allGames[y].score_1)+"\n";
                            send(Games::allGames[y].player_2.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                            Games::allGames.erase(Games::allGames.begin()+y);
                    }else if(Games::allGames[y].player_2.id == Players::allPlayers[i].id){
                        std::string tmpResponse = std::to_string(WIN_SURR)+"|"+std::to_string(Games::allGames[y].score_1)+"|"+std::to_string(Games::allGames[y].score_2)+"\n";
                        send(Games::allGames[y].player_1.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                        Games::allGames.erase(Games::allGames.begin()+y);
                    }
                }
                Players::allPlayers.erase(Players::allPlayers.begin()+i);
            }else{
                Players::allPlayers.erase(Players::allPlayers.begin()+i);
            }
        }
    }
    close(fd);
    FD_CLR( fd, &client_socks );
}

bool MyServer::correctPort(int port) {
    if ((port>0)&& (port<65000)){
        for(int i : invalidPorts){
            if (port == i){
                return false;
            }
        }
        return true;
    }
    return false;
}

void MyServer::clientDisconnected(int fd) {
    for(Player& tmpPlayer: Players::allPlayers){
        if (tmpPlayer.socket == fd){
            if (tmpPlayer.state == IN_GAME){
                for(GameRPS& tmpGame: Games::allGames){
                    if (tmpGame.player_1.id == tmpPlayer.id){
                        if (tmpGame.player_2.state == IN_GAME){
                        std::string tmpResponse = std::to_string(OPP_DISCONNECTED)+"\n";
                        send(tmpGame.player_2.socket, tmpResponse.c_str(), tmpResponse.length(),0);
                        tmpPlayer.state= DISCONNECTED;
                        }else if (tmpGame.player_2.state == DISCONNECTED){
                            for (int i = 0; i < Games::allGames.size(); ++i) {
                                if ((Games::allGames.at(i).player_1.id == tmpPlayer.id) || (Games::allGames.at(i).player_2.id == tmpPlayer.id)){
                                    for (int y = 0; y < Players::allPlayers.size(); ++y) {
                                        if ((Players::allPlayers.at(y).id == Games::allGames.at(i).player_1.id) || (Players::allPlayers.at(y).id == Games::allGames.at(i).player_2.id)){
                                            Players::allPlayers.erase(Players::allPlayers.begin()+y);
                                        }
                                    }
                                    Games::allGames.erase(Games::allGames.begin()+i);
                                }
                            }
                        }
                    }else if (tmpGame.player_2.id == tmpPlayer.id){
                        if (tmpGame.player_1.state == IN_GAME) {
                            std::string tmpResponse = std::to_string(OPP_DISCONNECTED) + "\n";
                            send(tmpGame.player_1.socket, tmpResponse.c_str(), tmpResponse.length(), 0);
                            tmpPlayer.state= DISCONNECTED;
                        }else if(tmpGame.player_1.state == DISCONNECTED){
                            for (int i = 0; i < Games::allGames.size(); ++i) {
                                if ((Games::allGames.at(i).player_1.id == tmpPlayer.id) || (Games::allGames.at(i).player_2.id == tmpPlayer.id)){
                                    for (int y = 0; y < Players::allPlayers.size(); ++y) {
                                        if ((Players::allPlayers.at(y).id == Games::allGames.at(i).player_1.id) || (Players::allPlayers.at(y).id == Games::allGames.at(i).player_2.id)){
                                            Players::allPlayers.erase(Players::allPlayers.begin()+y);
                                        }
                                    }
                                    Games::allGames.erase(Games::allGames.begin()+i);
                                }
                            }

                        }
                    }
                }
            } else{
                tmpPlayer.state = DISCONNECTED;
            }
        }

    }

    close(fd);
    FD_CLR( fd, &client_socks );
}




