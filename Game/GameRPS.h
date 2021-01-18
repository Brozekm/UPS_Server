//
// Created by mbrozek on 18.01.21.
//

#ifndef UPS_SEM_SERVER_GAMERPS_H
#define UPS_SEM_SERVER_GAMERPS_H

#define GAME_DATA 205
#include <utility>

#include "Player.h"


class GameRPS{
private:


public:
    Player player_1;
    int score_1;
    Player player_2;
    int score_2;

    GameRPS(Player pl1, Player pl2) : player_1(std::move(pl1)), score_1(0), player_2(std::move(pl2)),score_2(0){};

};

class Games{
public:
    static std::vector<GameRPS> allGames;
};

#endif //UPS_SEM_SERVER_GAMERPS_H
