#include <vector>
#include <sys/socket.h>
#include <iostream>
#include "GameRPS.h"

//
// Created by mbrozek on 18.01.21.
//
std::vector<GameRPS> Games::allGames;

Response Games::resolveMovesInGame(int gameIndex, int currentPlayerId) {
    Games::allGames.at(gameIndex).player_2.state = IN_GAME;
    Games::allGames.at(gameIndex).player_1.state = IN_GAME;
//    std::cout << "Valid option index 0: " << validMoves.at(0) << std::endl;
//    std::cout << "Valid option index 1: " << validMoves.at(1) << std::endl;
//    std::cout << "Valid option index 1: " << validMoves.at(2) << std::endl;
//    std::cout << "Player 1 option: " << Games::allGames.at(gameIndex).lastMove_1 << std::endl;
//    std::cout << "Player 2 option: " << Games::allGames.at(gameIndex).lastMove_2 << std::endl;
    if ((Games::allGames.at(gameIndex).lastMove_1 == Games::allGames.at(gameIndex).lastMove_2 )){
        return sameMoves(gameIndex, currentPlayerId);
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(0)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(1))){
        return addPointToPlayer2(gameIndex, currentPlayerId);
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(0)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(2))){
        return addPointToPlayer1(gameIndex, currentPlayerId);
    } else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(1)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(0))){
        return addPointToPlayer1(gameIndex, currentPlayerId);
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(1)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(2))){
        return addPointToPlayer2(gameIndex, currentPlayerId);
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(2)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(0))){
        return addPointToPlayer2(gameIndex, currentPlayerId);
    }else if ((Games::allGames.at(gameIndex).lastMove_1 == validMoves.at(2)) && (Games::allGames.at(gameIndex).lastMove_2 == validMoves.at(1))) {
        return addPointToPlayer1(gameIndex, currentPlayerId);
    }


    return Response(false, "ERROR RESOLVING GAME MOVES");
}

Response Games::addPointToPlayer2(int index, int playerId) {
    if(Games::allGames.at(index).player_2.id == playerId){
        Games::allGames.at(index).score_2++;
        if (Games::allGames.at(index).score_2 == WINS_FOR_VICTORY){
            GameRPS tmpGame = Games::allGames.at(index);
            std::string tmpResponse = std::to_string(LOSE)+"|"+std::to_string(tmpGame.score_1)+"|"+std::to_string(tmpGame.score_2)+"\n";
            send(tmpGame.player_1.socket, tmpResponse.c_str(), tmpResponse.length(),0);
            Games::allGames.erase(Games::allGames.begin()+index);
            return Response(true, std::to_string(WIN)+"|"+std::to_string(tmpGame.score_2)+"|"+std::to_string(tmpGame.score_1)+"\n");
        }
        std::string tmpResponse = std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_1)+"|"+std::to_string(Games::allGames.at(index).score_2)+"\n";
        send(Games::allGames.at(index).player_1.socket, tmpResponse.c_str(), tmpResponse.length(),0);
        return Response(true, std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_2)+"|"+std::to_string(Games::allGames.at(index).score_1)+"\n");

    }else if (Games::allGames.at(index).player_1.id == playerId){
        Games::allGames.at(index).score_2++;
        if (Games::allGames.at(index).score_2 == WINS_FOR_VICTORY){
            GameRPS tmpGame = Games::allGames.at(index);
            std::string tmpResponse = std::to_string(WIN)+"|"+std::to_string(tmpGame.score_2)+"|"+std::to_string(tmpGame.score_1)+"\n";
            send(tmpGame.player_2.socket, tmpResponse.c_str(), tmpResponse.length(),0);
            Games::allGames.erase(Games::allGames.begin()+index);
            return Response(true, std::to_string(LOSE)+"|"+std::to_string(tmpGame.score_1)+"|"+std::to_string(tmpGame.score_2)+"\n");
        }
        std::string tmpResponse = std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_2)+"|"+std::to_string(Games::allGames.at(index).score_1)+"\n";
        send(Games::allGames.at(index).player_2.socket, tmpResponse.c_str(), tmpResponse.length(),0);
        return Response(true, std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_1)+"|"+std::to_string(Games::allGames.at(index).score_2)+"\n");
    }


    return Response(false, "ERRROR GAME ADD POINT to player 2");
}

Response Games::addPointToPlayer1(int index, int playerId) {
    if(Games::allGames.at(index).player_2.id == playerId){
        Games::allGames.at(index).score_1++;
        if (Games::allGames.at(index).score_1 == WINS_FOR_VICTORY){
            GameRPS tmpGame = Games::allGames.at(index);
            std::string tmpResponse = std::to_string(WIN)+"|"+std::to_string(tmpGame.score_1)+"|"+std::to_string(tmpGame.score_2)+"\n";
            send(tmpGame.player_1.socket, tmpResponse.c_str(), tmpResponse.length(),0);
            Games::allGames.erase(Games::allGames.begin()+index);
            return Response(true, std::to_string(LOSE)+"|"+std::to_string(tmpGame.score_2)+"|"+std::to_string(tmpGame.score_1)+"\n");
        }
        std::string tmpResponse = std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_1)+"|"+std::to_string(Games::allGames.at(index).score_2)+"\n";
        send(Games::allGames.at(index).player_1.socket, tmpResponse.c_str(), tmpResponse.length(),0);
        return Response(true, std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_2)+"|"+std::to_string(Games::allGames.at(index).score_1)+"\n");

    }else if (Games::allGames.at(index).player_1.id == playerId){
        Games::allGames.at(index).score_1++;
        if (Games::allGames.at(index).score_1 == WINS_FOR_VICTORY){
            GameRPS tmpGame = Games::allGames.at(index);
            std::string tmpResponse = std::to_string(LOSE)+"|"+std::to_string(tmpGame.score_2)+"|"+std::to_string(tmpGame.score_1)+"\n";
            send(tmpGame.player_2.socket, tmpResponse.c_str(), tmpResponse.length(),0);
            Games::allGames.erase(Games::allGames.begin()+index);
            return Response(true, std::to_string(WIN)+"|"+std::to_string(tmpGame.score_1)+"|"+std::to_string(tmpGame.score_2)+"\n");
        }
        std::string tmpResponse = std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_2)+"|"+std::to_string(Games::allGames.at(index).score_1)+"\n";
        send(Games::allGames.at(index).player_2.socket, tmpResponse.c_str(), tmpResponse.length(),0);
        return Response(true, std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_1)+"|"+std::to_string(Games::allGames.at(index).score_2)+"\n");
    }

    return Response(false, "ERRROR GAME ADD POINT to player 1");
}

Response Games::sameMoves(int index, int playerId) {
    if (Games::allGames.at(index).player_1.id == playerId){
        std::string tmpResponse = std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_2)+"|"+std::to_string(Games::allGames.at(index).score_1)+"\n";
        send(Games::allGames.at(index).player_2.socket, tmpResponse.c_str(), tmpResponse.length(), 0);
        return Response(true, std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_1)+"|"+std::to_string(Games::allGames.at(index).score_2)+"\n");

    }else if (Games::allGames.at(index).player_2.id == playerId){
        std::string tmpResponse = std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_1)+"|"+std::to_string(Games::allGames.at(index).score_2)+"\n";
        send(Games::allGames.at(index).player_1.socket, tmpResponse.c_str(), tmpResponse.length(), 0);
        return Response(true, std::to_string(READY_FOR_PLAY)+"|"+std::to_string(Games::allGames.at(index).score_2)+"|"+std::to_string(Games::allGames.at(index).score_1)+"\n");
    }
    return Response(false, "ERROR SAME MOVES");
}
