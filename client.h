#ifndef client_h
#define client_h

#include <stdio.h> 
#include <math.h>
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include<netinet/in.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include<raylib.h>
#include "struct_data.h"

#define MAXLINE 1000
int client_main();
void localPlayer(Snake * snake);
void update_game();
Color getRandomColor();

#endif