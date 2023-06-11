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

// Shared data structures
/* char clientMessages[MAX_CLIENTS][BUFFER_SIZE]; */
int clientSockets[MAX_CLIENTS];
sem_t semaphore;

// Client thread function
void *clientThread(void *arg) {
    int clientSocket = *(int *)arg;
    while (1) {
        char message[BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, message, BUFFER_SIZE, 0);

        if (bytesRead <= 0) {
            // Client disconnected
            break;
        }
        
        // Acquire semaphore to access shared data
        sem_wait(&semaphore);
       /*  printf("Waiting: %d", *semaphore); */

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

        // Release semaphore
        sem_post(&semaphore);
      /*   printf("Posting : %d", *semaphore); */
    }

    // Close the socket and exit the thread
    close(clientSocket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stderr, "Port No not provided. Program terminated\n");
        exit(1);
    }
    int serverSocket, clientSocket;
    int portno;
    struct sockaddr_in serverAddress, clientAddress;
    pthread_t clientThreads[MAX_CLIENTS];

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

    // Initialize the semaphore
    if (sem_init(&semaphore, 0, 1) != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for connections...\n");

    // Accept incoming client connections and create client threads
    int clientCount = 0;
    while (clientCount < MAX_CLIENTS) {
        socklen_t clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);

        if (clientSocket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client %d connected: %s:%d\n",clientSocket-4, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

        // Acquire semaphore to access shared data
        sem_wait(&semaphore);

        // add the client socket to the list
        clientSockets[clientCount] = clientSocket;
        // relese semaphore
        sem_post(&semaphore);
        // Create a new client thread
        if (pthread_create(&clientThreads[clientCount], NULL, clientThread, (void *)&clientSocket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        clientCount++;
    }

    // Wait for all client threads to finish
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(clientThreads[i], NULL);
    }

    // Destroy the semaphore
    sem_destroy(&semaphore);

    // Close the server socket
    close(serverSocket);

    return 0;
}
