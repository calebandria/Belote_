#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "card_piling.h"

#define BUFFER_SIZE 1024

void deserializeScarte(struct Scarte *scarte, char *buffer) {
    memcpy(&(scarte->valeur), buffer, sizeof(int));
    memcpy(&(scarte->couleur), buffer + sizeof(int), sizeof(char));
}

void deserializePlayer(struct Player *player, char *buffer) {
    memcpy(&(player->main), buffer, sizeof(int));
    memcpy(&(player->jeuChoisi), buffer + sizeof(int), sizeof(int));
    int numCartes = sizeof(struct Scarte);
    for (int i = 0; i < numCartes; i++) {
        deserializeScarte(&(player->cartes[i]), buffer + (2 * sizeof(int)) + (i * (sizeof(int) + sizeof(char))));
    }
}

void *receiveThread(void *arg){
    int serverSocket = *(int *)arg;
    char message[BUFFER_SIZE];

    while (1) {
        ssize_t bytesRead = recv(serverSocket, message, BUFFER_SIZE, 0);

        if (bytesRead <= 0) {
            // Server disconnected
            printf("Server disconnected");
            break;
        }
    
            printf("Server: %s\n", message);
     }

    // Close the socket and exit the thread
    close(serverSocket);
    pthread_exit(NULL);
}

void *receiveStructure(void *arg){

    Player receivedPlayer;
    int serverSocket = *(int*) arg;
    char buffer[sizeof(Player)];
    
    while(1){
        ssize_t byteReceived = recv(serverSocket, buffer, sizeof(Player),0);

        if(byteReceived <=0){
            // server disconnected
            printf("Server disconnected");
            break;
        }
        deserializePlayer(&receivedPlayer, buffer);
        displayCardForEachPlayer(receivedPlayer);
    }   
    
    close(serverSocket);
    pthread_exit(NULL); 

}

int main(int argc, char* argv[]) {
    int serverSocket;
    int portno;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    pthread_t receiveThreadId;

    if(argc < 3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }
    
    portno = atoi(argv[2]);
    
    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    server = gethostbyname(argv[1]);
    if(server ==NULL){
        fprintf(stderr, "Error, no such host");
    }

    bzero((char *) &serverAddress, sizeof(serverAddress));
    // Set up server address
    serverAddress.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(portno);

    // Connect to the server
    if (connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server. Start chatting!\n");

    // Create a thread to receive messages from the server
  /*   if (pthread_create(&receiveThreadId, NULL, receiveThread, (void *)&serverSocket) != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    } */
       if (pthread_create(&receiveThreadId, NULL, receiveStructure, (void *)&serverSocket) != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    // Send messages to the server
  /*   char message[BUFFER_SIZE];
    while (1) {
        printf("You: ");
        fgets(message, BUFFER_SIZE, stdin);

        // Remove newline character
        message[strcspn(message, "\n")] = '\0';

        // Send the message to the server
        ssize_t bytesSent = send(serverSocket, message, strlen(message)+1, 0);
        if (bytesSent < 0) {
            perror("Send failed");
            break;
        }
    }
 */
    // Wait for the receive thread to finish
    pthread_join(receiveThreadId, NULL);

    // Close the socket
    close(serverSocket);

    return 0;
}
