//
// Created by mbrozek on 18.01.21.
//

#ifndef UPS_SEM_SERVER_PLAYER_H
#define UPS_SEM_SERVER_PLAYER_H

#include <string>
#include <utility>
#include <vector>

#define NOT_IN_GAME 0
#define IN_GAME 1
#define WAIT 2
#define DISCONNECTED 3

class Player{
private:
    int getNewId();
public:
    int id;
    std::string nick;
    int state;
    int socket;
    Player(std::string  nNick, int nState, int nSocket) : id(getNewId()), nick(std::move(nNick)), state(nState), socket(nSocket){};
};

class Players{
public:
    static std::vector<Player> allPlayers;
};



#endif //UPS_SEM_SERVER_PLAYER_H
