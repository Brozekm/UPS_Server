//
// Created by mbrozek on 17.01.21.
//


#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <regex>
#include "CommunicationParser.h"
#include "../Game/Player.h"
#include "../Game/GameRPS.h"

Response CommunicationParser::clientReqParser(int fd, const std::string &request) {
    ClientReq clReq = parseRequest(request);

    if(clReq.type !=-1){
        switch (clReq.type) {
            case PING:{
                return Response(true, std::to_string(PING_SUCCESS)+"\n");
            }
            case DISCONNECT:{
                return exitClinet(clReq.id, clReq.param);
            }
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
            case FIND_GAME:
                return findGame(clReq.id, clReq.param);
            default:
                Response unknownReq(false, std::to_string(UNKNOWN_REQUEST)+"\n");
                return unknownReq;
        }
    }

    return Response (false, std::to_string(WRONG_PROTOCOL)+"\n");
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
        return ClientReq(-1,-1,"");
    }
    if(!std::regex_match(cliParams.at(2), std::regex("[a-zA-Z0-9]*"))){
        return ClientReq(-1,-1,"");
    }
    try {
        int type = std::stoi(cliParams.at(0));
        int id = std::stoi(cliParams.at(1));
        if (!cliParams.at(2).empty()){
            if (cliParams.at(2).length()<=16){
                return ClientReq (type,id,cliParams.at(2));
            }

        }

        return  ClientReq(type,id,cliParams.at(2));
    } catch (...) {
        std::cout << "Parameter is not expected data type" << std::endl;
        return ClientReq(-1,-1,"");
    }


}

Response CommunicationParser::loginPlayer(int fd, int id, const std::string& params) {
    if(id ==0 ){
        if ((params.length() < 3) || (params.length() > 16)){
            return Response(false, std::to_string(INVALID_NICK)+"\n");
        }
         for(Player &i: Players::allPlayers){
             if (i.nick == params){
                 if (i.state ==DISCONNECTED){
                     i.socket = fd;
                     i.state = NOT_IN_GAME;
                     for(GameRPS& oneGame: Games::allGames){
                         if(oneGame.player_1.id == i.id){
                             i.state = IN_GAME;
                             oneGame.player_1 = i;
                             std::string tmpResponse = std::to_string(GAME_DATA)+"|"+std::to_string(oneGame.score_2)+"|"+std::to_string(oneGame.score_1)+"|"+oneGame.player_1.nick+"\n";
                             send(oneGame.player_2.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                             return Response(true, std::to_string(RECONNECTED)+"|"+std::to_string(i.id)+"|"+std::to_string(i.state)+"\n");

                         }else if (oneGame.player_2.id == i.id){
                             i.state = IN_GAME;
                             oneGame.player_2 = i;
                             std::string tmpResponse = std::to_string(GAME_DATA)+"|"+std::to_string(oneGame.score_1)+"|"+std::to_string(oneGame.score_2)+"|"+oneGame.player_2.nick+"\n";
                             send(oneGame.player_1.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                             return Response(true, std::to_string(RECONNECTED)+"|"+std::to_string(i.id)+"|"+std::to_string(i.state)+"\n");
                         }
                     }
                     return Response(true, std::to_string(RECONNECTED)+"|"+std::to_string(i.id)+"|"+std::to_string(i.state)+"\n");
                 }else{
                     return Response(true, std::to_string(NICK_IS_USED)+"\n");
                 }
             }
         }

        Player newPlayer(params, NOT_IN_GAME, fd);
        for(Player& i : Players::allPlayers){
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
    return Response(false, std::to_string(PLAYER_HAVE_NO_GAME)+"\n");
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
    std::cout << "Logging out failed for player ("<< nick << ", id: " << id << ")" << std::endl;
    return Response(false, std::to_string(UNSUCCESSFUL_LOGOUT)+"\n");
}



Response CommunicationParser::playGame(int id, const std::string& move) {
    int notValid = -1;
    for(const std::string& validMove : Games::validMoves){
        if (validMove == move){
            notValid=1;
        }
    }
    if (notValid<0){
        return Response(false, std::to_string(UNKNOWN_MOVE)+"\n");
    }

    for (int i = 0; i < Games::allGames.size(); ++i) {
        if (id == Games::allGames.at(i).player_1.id){
            if (Games::allGames.at(i).player_1.state == IN_GAME){
                if (Games::allGames.at(i).player_2.state == WAIT){
                    Games::allGames.at(i).lastMove_1 = move;
                    return Games::resolveMovesInGame(i, id);
                }
                Games::allGames.at(i).lastMove_1 = move;
                Games::allGames.at(i).player_1.state = WAIT;
                return Response(true, std::to_string(WAIT_FOR_OPP_MOVE)+"\n");
            }

        } else if (id == Games::allGames.at(i).player_2.id){
            if(Games::allGames.at(i).player_2.state == IN_GAME){
                if (Games::allGames.at(i).player_1.state == WAIT){
                    Games::allGames.at(i).lastMove_2 = move;
                    return Games::resolveMovesInGame(i, id);
                }
                Games::allGames.at(i).lastMove_2 = move;
                Games::allGames.at(i).player_2.state = WAIT;
                return Response(true, std::to_string(WAIT_FOR_OPP_MOVE)+"\n");
            }

        }
    }

    return Response(false,std::to_string(PLAYER_HAVE_NO_GAME)+"\n");
}


Response CommunicationParser::surrender(int id, const std::string& nick) {
    for (int i = 0; i < Games::allGames.size(); ++i) {
        if(Games::allGames.at(i).player_1.id == id){
            if (nick == Games::allGames.at(i).player_1.nick){
                int sc1 = Games::allGames.at(i).score_1;
                int sc2 = Games::allGames.at(i).score_2;
                for (int j = 0; j < Players::allPlayers.size(); ++j) {
                    if (Players::allPlayers.at(j).id == Games::allGames.at(i).player_2.id){
                        if (Players::allPlayers.at(j).state == DISCONNECTED){
                                Players::allPlayers.erase(Players::allPlayers.begin()+j);
                            Games::allGames.erase(Games::allGames.begin()+i);
                            return Response(true, std::to_string(DRAW)+"|"+std::to_string(sc1)+"|"+std::to_string(sc2)+"\n");
                        }
                    }
                }
                std::string tmpResponse = std::to_string(WIN_SURR)+"|"+std::to_string(sc2)+"|"+std::to_string(sc1)+"\n";
                send(Games::allGames.at(i).player_2.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                Games::allGames.erase(Games::allGames.begin()+i);
                return Response(true, std::to_string(LOSE)+"|"+std::to_string(sc1)+"|"+std::to_string(sc2)+"\n");
            }
        } else if (Games::allGames.at(i).player_2.id == id){
            if (nick == Games::allGames.at(i).player_2.nick){
                int sc1 = Games::allGames.at(i).score_1;
                int sc2 = Games::allGames.at(i).score_2;
                for (int j = 0; j < Players::allPlayers.size(); ++j) {
                    if (Players::allPlayers.at(j).id == Games::allGames.at(i).player_1.id){
                        if (Players::allPlayers.at(j).state == DISCONNECTED){
                            Players::allPlayers.erase(Players::allPlayers.begin()+j);
                            Games::allGames.erase(Games::allGames.begin()+i);
                            return Response(true, std::to_string(DRAW)+"|"+std::to_string(sc2)+"|"+std::to_string(sc1)+"\n");
                        }
                    }
                }

                std::string tmpResponse = std::to_string(WIN_SURR)+"|"+std::to_string(sc1)+"|"+std::to_string(sc2)+"\n";
                send(Games::allGames.at(i).player_1.socket, tmpResponse.c_str(),tmpResponse.length(),0);
                Games::allGames.erase(Games::allGames.begin()+i);
                return Response(true, std::to_string(LOSE)+"|"+std::to_string(sc2)+"|"+std::to_string(sc1)+"\n");
            }
        }

    }
    std::cout << "Surrender committed by player ("<< nick << ", id: " << id << ") failed." << std::endl;
    return Response(false, std::to_string(UNSUCCESSFUL_SURR)+"\n");
}

Response CommunicationParser::findGame(int id, const std::string& nick) {
    for (GameRPS tmpGame: Games::allGames){
        if ((tmpGame.player_2.id == id)||(tmpGame.player_1.id == id)){
            return Response(false, std::to_string(PLAY_STILL_IN_GAME)+"\n");
        }
    }
    for(Player& current : Players::allPlayers){
        if (current.id == id){
            if(current.nick == nick){
                if (current.state == IN_GAME){
                    for(Player& rand : Players::allPlayers){
                        if(rand.state == NOT_IN_GAME){
                            rand.state = IN_GAME;
                            Games::allGames.emplace_back(GameRPS(current,rand));
                            Response tmpResponse = getGameData(rand.id, rand.nick);
                            send(rand.socket, tmpResponse.response.c_str(), tmpResponse.response.length(),0);
                            current.state = IN_GAME;
                            return Response(true, std::to_string(NEW_PLAYER_CREATED)+"|"+std::to_string(current.id)+"|"+std::to_string(current.state)+"\n");
                        }
                    }
                    current.state = NOT_IN_GAME;
                    return Response(true, std::to_string(LOOKING_FOR_NEW_GAME)+"\n");
                }

            }
        }
    }

    return Response(false, std::to_string(PLAYER_NO_LONGER_EXITS)+"\n");
}

Response CommunicationParser::exitClinet(int id, const std::string& nick) {
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
                return Response(false, std::to_string(SUCCESSFUL_DISCONNECT)+ "\n");
            }
        }
    }
    std::cout << "Logging out failed for player ("<< nick << ", id: " << id << ")" << std::endl;
    return Response(false, std::to_string(UNSUCCESSFUL_DISCONNECT)+"\n");
}



