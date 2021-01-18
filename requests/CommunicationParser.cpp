//
// Created by mbrozek on 17.01.21.
//


#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include "CommunicationParser.h"
#include "../Game/Player.h"
#include "../Game/GameRPS.h"

Response CommunicationParser::clientReqParser(int fd, const std::string &request) {
    ClientReq clReq = parseRequest(request);

    if(clReq.type !=-1){
        switch (clReq.type) {
            case LOGIN:
                return loginPlayer(fd, clReq.id,clReq.param);
            case LOGOUT:
                return logoutPlayer(clReq.id, clReq.param);
            case PLAY:
                return playGame(clReq.id,clReq.param);
            case GET_INIT_GAME_DATA:
                return getGameData(clReq.id, clReq.param);
            case SURRENDER:
                return surrender(clReq.id, clReq.param);
            default:
                Response unknownReq(false, std::to_string(UNKNOWN_REQUEST)+"\n");
                return unknownReq;
        }
    }
    Response wrongProtocol(false, std::to_string(WRONG_PROTOCOL)+"\n");
    return wrongProtocol;
}

ClientReq CommunicationParser::parseRequest(const std::string &request) {
    std::vector<std::string> cliParams;
    std::stringstream streamReq(request);
    std::string tmpStr;

    while (std::getline(streamReq,tmpStr, '|')){
        cliParams.push_back(tmpStr);
    }
    if(cliParams.size() != 3 ){
        std::cout << "Wrong number of parameters" << std::endl;
        ClientReq cliReq(-1,-1, "");
        return cliReq;
    }
    try {
        int type = std::stoi(cliParams.at(0));
        int id = std::stoi(cliParams.at(1));
        ClientReq cliReq(type,id,cliParams.at(2));
        return cliReq;
    } catch (...) {
        std::cout << "Parameter is not expected data type" << std::endl;
        ClientReq cliReq(-1,-1,"");
        return cliReq;
    }


}

Response CommunicationParser::loginPlayer(int fd, int id, const std::string& params) {
    if(id ==0 ){
        if (params.length() < 3){
            return Response(false, std::to_string(INVALID_NICK)+"\n");
        }
         for(Player &i: Players::allPlayers){
             if (i.nick.compare(params) == 0){
                 if (i.state ==DISCONNECTED){
                     i.socket = fd;
                     i.state = NOT_IN_GAME;
                     for(const GameRPS& oneGame: Games::allGames){
                         if((oneGame.player_1.id == i.id)||(oneGame.player_2.id == id)){
                             i.state = IN_GAME;
                         }
                     }
                     return Response(true, std::to_string(RECONNECTED)+"|"+std::to_string(i.id)+"|"+std::to_string(i.state)+"\n");
                 }else{
                     return Response(true, std::to_string(NICK_IS_USED)+"\n");
                 }
             }
         }

        Player newPlayer(params, NOT_IN_GAME, fd);
        for(Player i : Players::allPlayers){
            if(i.state == NOT_IN_GAME){
                i.state = IN_GAME;
                newPlayer.state = IN_GAME;
                Games::allGames.emplace_back(GameRPS(i,newPlayer));
                Response tmpResponse = getGameData(i.id,i.nick);
                send(i.socket, tmpResponse.response.c_str(),tmpResponse.response.length(),0);
            }
        }
        Players::allPlayers.push_back(newPlayer);
        std::cout<< "New player ("<< newPlayer.nick << ", id: "<< newPlayer.id <<") added successfully" << std::endl;
        return Response(true, std::to_string(NEW_PLAYER_CREATED)+"|"+std::to_string(newPlayer.id)+"|"+std::to_string(newPlayer.state)+"\n");

    } else
        return Response(false, std::to_string(WRONG_PLAYER_ID)+"\n");

}

Response CommunicationParser::getGameData(int id, const std::string& nick) {
    for(const GameRPS& tmpGame: Games::allGames){
        if (id == tmpGame.player_1.id){
            return Response(true, std::to_string(GAME_DATA)+"|"+std::to_string(tmpGame.score_1)+"|"+std::to_string(tmpGame.score_2)+"|"+tmpGame.player_2.nick+"\n");
        } else if (id == tmpGame.player_2.id){
            return Response(true, std::to_string(GAME_DATA)+"|"+std::to_string(tmpGame.score_2)+"|"+std::to_string(tmpGame.score_1)+"|"+tmpGame.player_1.nick+"\n");
        }
    }
    return Response(false, std::to_string(PLAYER_HAVE_NO_GAME));
}

Response CommunicationParser::logoutPlayer(int id, const std::string& nick) {
    for(int i = 0 ; i< Players::allPlayers.size(); i++){
        if(id == Players::allPlayers.at(i).id){
            if (nick==Players::allPlayers.at(i).nick){
                if (Players::allPlayers.at(i).state == 1){
                    for(int y = 0 ; y < Games::allGames.size(); y++){
                        if(Games::allGames[y].player_1.id == id){
                            std::string tmpResponse = std::to_string(WIN_SURR)+"|"+std::to_string(Games::allGames[y].score_2)+"|"+std::to_string(Games::allGames[y].score_1)+"\n";
                            send(Games::allGames[y].player_2.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                            Games::allGames.erase(Games::allGames.begin()+y);
                        }else if(Games::allGames[y].player_2.id == id){
                            std::string tmpResponse = std::to_string(WIN_SURR)+"|"+std::to_string(Games::allGames[y].score_1)+"|"+std::to_string(Games::allGames[y].score_2)+"\n";
                            send(Games::allGames[y].player_1.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                            Games::allGames.erase(Games::allGames.begin()+y);
                        }
                    }
                }
                std::cout<<"Player ("<< Players::allPlayers.at(i).nick << ", id: " <<Players::allPlayers.at(i).id << ") logged out." << std::endl;
                Players::allPlayers.erase(Players::allPlayers.begin()+i);
                return Response(true, std::to_string(SUCCESSFUL_LOGOUT)+ "\n");
            }
        }
    }
    return Response(false, std::to_string(UNSUCCESSFUL_LOGOUT)+"\n");
}

Response CommunicationParser::playGame(int id, const std::string& move) {
    return Response(false,"WRONG");
}

Response CommunicationParser::surrender(int id, const std::string& nick) {

    return Response(false, __cxx11::basic_string());
}



