#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "handle_tracker.h"

#define MAX_JOBS 1000
#define NUM_THREADS 10

struct Job
{
    int socket;
};

struct Job jobQueue[MAX_JOBS];
int jobCount = 0;

pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueCondition = PTHREAD_COND_INITIALIZER;

void enqueueJob(struct Job job)
{
    pthread_mutex_lock(&queueMutex);

    while (jobCount == MAX_JOBS)
    {
        pthread_cond_wait(&queueCondition, &queueMutex);
    }

    jobQueue[jobCount++] = job;

    pthread_cond_signal(&queueCondition);
    pthread_mutex_unlock(&queueMutex);
}

struct Job dequeueJob()
{
    pthread_mutex_lock(&queueMutex);

    while (jobCount == 0)
    {
        pthread_cond_wait(&queueCondition, &queueMutex);
    }

    struct Job job = jobQueue[--jobCount];

    pthread_cond_signal(&queueCondition);
    pthread_mutex_unlock(&queueMutex);

    return job;
}

void *workerThread(void *arg)
{
    (void)arg; 
    pthread_t threadId = pthread_self();

    while (1)
    {
        struct Job job = dequeueJob();
        int client_socket = job.socket;

        printf("Thread %lu is handling a new job\n", (unsigned long)threadId);

        while (1)
        {
            char buffer[1024] = {0};
            int valread = read(client_socket, buffer, 1024);

            if (valread <= 0)
            {
                // Le client a fermé la connexion
                break;
            }

            // Supprimer le caractère de nouvelle ligne à la fin du message
            buffer[strcspn(buffer, "\n")] = '\0';

            printf("Thread %lu - Reçu : %s\n", (unsigned long)threadId, buffer);

            if (strncmp(buffer, "announce", 8) == 0)
            {
                handleAnnounce(client_socket, buffer);
            }
            if (strncmp(buffer, "look", 4) == 0)
            {
                handleLook(client_socket, buffer);
            }
            if (strncmp(buffer, "getfile", 7) == 0)
            {
                handleGetFile(client_socket, buffer);
            }
            if (strncmp(buffer, "info", 4) == 0)
            {
                handleGetFileInfoFromTracker(client_socket, buffer);
            }
            if (strncmp(buffer, "update", 6) == 0)
            {
                handleUpdate(client_socket, buffer);
            }
        }

        printf("Thread %lu - Thread términé\n", (unsigned long)threadId);
        close(client_socket);
    }

    return NULL;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t threads[NUM_THREADS];

    // Créer un socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurer les options du socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Lier le socket à l'adresse
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Écouter les connexions entrantes
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    init();

    // Créer le pool de threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, workerThread, NULL);
    }

    printf("Le tracker est en attente de connexions...\n");

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        struct Job newJob;
        newJob.socket = new_socket;
        enqueueJob(newJob);
    }
    // endFree();

    return 0;
}