#include <vector>
#include "GameRPS.h"

//
// Created by mbrozek on 18.01.21.
//
std::vector<GameRPS> Games::allGames;

Response Games::resolveMovesInGame(int gameIndex) {
    if ((Games::allGames.at(gameIndex).lastMove_1 == Games::allGames.at(gameIndex).lastMove_2 )){
        //TODO DRAW
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(1)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(2))){
//        TODO PLAYER 2 ADD POINT - check for win
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(1)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(3))){
//        TODO PLAYER 1 ADD POINT - check for win
    } else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(2)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(1))){
//        TODO PLAYER 1 ADD POINT - check for win
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(2)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(3))){
//        TODO PLAYER 2 ADD POINT - check for win
    }


    return Response(false, "ERROR RESOLVING GAME MOVES");
}
