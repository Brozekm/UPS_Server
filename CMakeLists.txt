cmake_minimum_required(VERSION 3.17)
project(UPS_sem_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(ups_server main.cpp requests/OutGoing.cpp requests/OutGoing.h server/MyServer.cpp server/MyServer.h requests/CommunicationParser.h requests/CommunicationParser.cpp Game/GameRPS.h Game/GameRPS.cpp Game/Player.h Game/Player.cpp)