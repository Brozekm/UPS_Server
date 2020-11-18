//
// Created by martin on 18.11.20.
//

#ifndef UPS_SEM_SERVER_MYSERVER_H
#define UPS_SEM_SERVER_MYSERVER_H


#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include <regex>

class MyServer {
public:
    MyServer();
    int runServer();

private:
    int server_socket;
    int client_socket, fd;
    int return_value;
    char cbuf [32];
    int len_addr;
    int a2read;
    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, tests;

    int init();
};


#endif //UPS_SEM_SERVER_MYSERVER_H
