#ifndef MY_SERVER
#define MY_SERVER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "card_piling.h"

#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024

#include "myServer_clean.h"
#include "card_piling.h"

// shared resources

int clientSockets[MAX_CLIENTS];
char buffer[sizeof(Player)];

sem_t semForSockets;
sem_t semForBuffer;

void serializeScarte(Scarte scarte, char* buffer){
    memcpy(buffer, &(scarte.valeur), sizeof(int));
    memcpy(buffer + sizeof(int), &(scarte.couleur), sizeof(char));
}

void serializePlayer(Player player, char *buffer){
    memcpy(buffer, &(player.main), sizeof(int));
    memcpy(buffer + sizeof(int), &(player.jeuChoisi), sizeof(int));
 /*    int numCartes = sizeof(Scarte); */
    for(int i = 0; i < CARDS_PLAYER; i++){
        serializeScarte(player.cartes[i], buffer + (2* sizeof(int)) + (i* (sizeof(int)+ sizeof(char))));
    }
}

void *sendStructure(void *arg){
    int clientSocket = *(int *)arg;

// initialise chaque structure de joueur
    Player players[MAX_CLIENTS];
    Pile* pile;
  /*   int playerHand;  */
    Scarte *cardToPlay = playableCards();

    shuffleCard(cardToPlay);
    pile = malloc(sizeof(Pile));


    //construction de la pile de 32 cartes
    for(int i=0; i< CARD_NUMBER;i++){
        empiler(pile, &(cardToPlay[i]));
    }
    printf("Empilement done!\n");
/*     Player *players = malloc(PLAYER_NUMBER*sizeof(Player)); */
    players[0].main = true;

    shareCards(pile, players, getIndexPlayerhand(players));

   /*  sem_wait(&semForSockets); */
   /*  sem_wait(&semForBuffer); */
    for(int i=0;i<MAX_CLIENTS;i++){
        sem_wait(&semForBuffer);
        serializePlayer(players[i], buffer);
        printf("Serialized done!\n");
        sem_wait(&semForSockets);
        if(clientSockets[i]!=0){
            sleep(5);
            ssize_t bytesSent = send(clientSockets[i],buffer, sizeof(int)+sizeof(int)+ (CARDS_PLAYER* (sizeof(int)+sizeof(char))),0);
            printf("Sockets working?\n");
            if(bytesSent < 0){
                perror("Send failed");
                break;
            }
            
        }
        sem_post(&semForSockets);
        
        
    }
    sem_post(&semForBuffer);
 /*    sem_post(&semForSockets); */

    // close the socket and exit thread
    close(clientSocket);
    pthread_exit(NULL);
    
}

void *clientThread(void *arg) {
    int clientSocket = *(int *)arg;
    while (1) {
        char message[BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, message, BUFFER_SIZE, 0);

        if (bytesRead <= 0) {
            // Client disconnected
            break;
        }
        
        // Acquire semForSockets to access shared data
        sem_wait(&semForSockets);
       /*  printf("Waiting: %d", *semForSockets); */

        // Store the client's message in the shared data structure

        /* strcpy(clientMessages[clientSocket-4], message);
        printf("Client %d: %s\n", clientSocket-4, clientMessages[clientSocket-4]); */

        // broadcast the client's message to all other clients, including the server
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clientSockets[i]!=clientSocket){
                ssize_t bytesSent = send(clientSockets[i], message, bytesRead,0);
                if(bytesSent < 0){
                    perror("Send failed");
                    break;
                }
            }
        }

        // Release semForSockets
        sem_post(&semForSockets);
      /*   printf("Posting : %d", *semForSockets); */
    }

    // Close the socket and exit the thread
    close(clientSocket);
    pthread_exit(NULL);
}

int serverSocket(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Port No not provided. Program terminated");
        exit(1);
    }
    int serverSocket;
    int portno;
    struct sockaddr_in serverAddress;
    
    // Create a server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // clear data
    bzero((char *) &serverAddress, sizeof(serverAddress));

    // Writing port
    portno = atoi(argv[1]);

    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portno);

    // Bind the server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for client connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for connections...\n");

       // Initialize the semForSockets
    if (sem_init(&semForSockets, 0, 1) != 0) {
        perror("semForSockets initialization failed");
        exit(EXIT_FAILURE);
    }
     // Initialize the semForBuffer

    if(sem_init(&semForBuffer, 0, 1)!=0){
        perror("semForSockets initialization failed");
        exit(EXIT_FAILURE);
    }

    return serverSocket;
}

void clientConnecting(int serverSocket){

    int clientSocket;
    pthread_t clientThreads[MAX_CLIENTS];
    struct sockaddr_in clientAddress;
    // Accept incoming client connections and create client threads
    int clientCount = 0;

    while (clientCount < MAX_CLIENTS){
        socklen_t clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        
        if (clientSocket < 0){
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Client %d connected: %s:%d\n",clientSocket-4, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
         // Acquire semForSockets to access shared data
      /*   sem_wait(&semForSockets); */
        
        
        // add the client socket to the list
        clientSockets[clientCount] = clientSocket;
        
        // relese semForSockets
        /* sem_post(&semForBuffer); */
        /* sem_post(&semForBuffer); */
        printf("Post buffer works here!\n");
        // Create a new client thread
        /* if (pthread_create(&clientThreads[clientCount], NULL, clientThread, (void *)&clientSocket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        } *//* 
        sem_wait(&semForBuffer); */

        if(pthread_create(&clientThreads[clientCount], NULL, sendStructure, (void *)&clientSocket) != 0){
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        printf("Thread creation %d done\n", clientCount);
        /* sem_wait(&semForBuffer); */
       /*  sem_post(&semForSockets); */

        clientCount++;
    }

    // Wait for all client threads to finish
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(clientThreads[i], NULL);
    }
    
    // Destroy the semForSockets
    sem_destroy(&semForSockets);
    sem_destroy(&semForBuffer);

    // Close the server socket
    close(serverSocket);
}

#endif
