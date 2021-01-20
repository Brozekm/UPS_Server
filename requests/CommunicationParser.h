//
// Created by mbrozek on 17.01.21.
//

#ifndef UPS_SEM_SERVER_COMMUNICATIONPARSER_H
#define UPS_SEM_SERVER_COMMUNICATIONPARSER_H
#include <string>
#include <utility>
#include <vector>

//Successful CODES
#define NEW_PLAYER_CREATED 202
#define RECONNECTED 203
#define SUCCESSFUL_LOGOUT 204
#define GAME_DATA 205
#define WAIT_FOR_OPP_MOVE 206
#define LOSE 207
#define WIN 208
#define WIN_SURR 209
#define READY_FOR_PLAY 210
#define LOOKING_FOR_NEW_GAME 211
#define NICK_IS_USED 220

//Error codes
#define WRONG_PROTOCOL 400
#define UNKNOWN_REQUEST 401
#define UNKNOWN_MOVE 402
#define WRONG_PLAYER_ID 410
#define INVALID_NICK 411
#define PLAYER_HAVE_NO_GAME 412
#define UNSUCCESSFUL_LOGOUT 413
#define UNSUCCESSFUL_SURR 414
#define PLAY_STILL_IN_GAME 415
#define PLAYER_NO_LONGER_EXITS 416
// Types of requests
#define LOGIN 2
#define LOGOUT 3
#define PLAY 4
#define GET_INIT_GAME_DATA 5
#define SURRENDER 6
#define FIND_GAME 7

class ClientReq{
public:
    int type;
    int id;
    std::string param;
    ClientReq(int reqType, int reqId, std::string reqOtherPar) : type(reqType), id(reqId), param(std::move(reqOtherPar)){};
};

class Response{
public:
  bool successful;
  std::string response;
  Response(bool succ, std::string resp) : successful(succ), response(std::move(resp)){};
};

class CommunicationParser{
private:
    static ClientReq parseRequest(const std::string& request);
    static Response getGameData(int id, const std::string& basicString);
    static Response loginPlayer(int fd, int id, const std::string& basicString);
    static Response logoutPlayer(int id, const std::string& basicString);
    static Response playGame(int id, const std::string& move);
    static Response surrender(int id, const std::string& basicString);
    static Response findGame(int id, const std::string& basicString);
public:
    static Response clientReqParser(int fd, const std::string& request);
};

#endif //UPS_SEM_SERVER_COMMUNICATIONPARSER_H
