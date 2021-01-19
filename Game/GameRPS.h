//
// Created by mbrozek on 18.01.21.
//

#ifndef UPS_SEM_SERVER_GAMERPS_H
#define UPS_SEM_SERVER_GAMERPS_H


#include <utility>

#include "Player.h"
#include "../requests/CommunicationParser.h"


class GameRPS{
private:


public:
    Player player_1;
    int score_1;
    std::string lastMove_1;
    Player player_2;
    int score_2;
    std::string lastMove_2;

    GameRPS(Player pl1, Player pl2) : player_1(std::move(pl1)), score_1(0), lastMove_1("none"), player_2(std::move(pl2)),score_2(0), lastMove_2("none"){};

};

class Games{
public:
    static std::vector<GameRPS> allGames;
    inline static const std::vector<std::string> validMoves {"Rock", "Paper", "Scissors"};
    static Response resolveMovesInGame(int gameIndex);
};

#endif //UPS_SEM_SERVER_GAMERPS_H
