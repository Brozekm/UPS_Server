//
// Created by mbrozek on 18.01.21.
//

#include "Player.h"
std::vector<Player> Players::allPlayers;
int Player::getNewId() {
    if(Players::allPlayers.empty()){
        return 1;
    }else
    return Players::allPlayers.at(Players::allPlayers.size()-1).id +1;
}

