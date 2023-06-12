// Shared data structures
/* char clientMessages[MAX_CLIENTS][BUFFER_SIZE]; */

void *clientThread(void *arg);
int serverSocket(int argc, char* argv[]);
void clientConnecting(int serverSocket);