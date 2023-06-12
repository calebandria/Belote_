#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 3

int sharedVariable = 0;
sem_t semaphore;

void *threadFunction(void *arg) {
    int threadId = *(int *)arg;

    // Perform some work before accessing the shared resource

    // Acquire the semaphore to access the shared resource
    sem_wait(&semaphore);

    // Access the shared resource (critical section)
    sharedVariable++;
    printf("Thread %d updated sharedVariable: %d\n", threadId, sharedVariable);
    sleep(20);
    // Release the semaphore to allow other threads to access the shared resource
    sem_post(&semaphore);

    // Perform some work after accessing the shared resource

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int threadIds[NUM_THREADS];

    // Initialize the semaphore
    sem_init(&semaphore, 0, 1);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, threadFunction, &threadIds[i]);

    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy the semaphore
    sem_destroy(&semaphore);

    return 0;
}
