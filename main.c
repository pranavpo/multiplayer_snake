#include <stdio.h>
#include <string.h>
#include "server.h"
#include "client.h"

int main(){
    char string[50];
    printf("Are you Client(client) or Server(server)?\n");
    scanf("%s",string);
    if(strcmp(string,"server") == 0){
        server_main();
    }else if(strcmp(string,"client") == 0){
        client_main();
    }   
    return 0;
}