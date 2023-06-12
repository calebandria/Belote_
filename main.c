#include "myServer_clean.h"
#include "card_piling.h"

int main(int argc, char *argv[]){
    int server=serverSocket(argc, argv);
    clientConnecting(server);

}