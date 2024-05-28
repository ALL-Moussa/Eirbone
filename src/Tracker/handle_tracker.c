#include "handle_tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

struct Files *fileList;
struct Peers *peerList;

// ####################################################################
// ########             Fonctions d'affichage                   #######
// ####################################################################

// Fonction pour afficher une structure de type File
void printFile(struct File *file)
{
    printf("Nom: %s\n", file->nom);
    printf("Chemin: %s\n", file->path);
    printf("Clé: %s\n", file->key);
    printf("Taille: %d\n", file->size);
    printf("Taille de pièce: %d\n", file->piecesize);
}

// Fonction pour afficher structure Files
void printFiles(struct Files *files)
{
    for (unsigned int i = 0; i < files->size; ++i)
    {
        printFile(files->files[i]);
    }
}

// ####################################################################
// ########             Fonctions utilitaires                   #######
// ####################################################################

// Initialiser la liste de fichiers
void init()
{
    peerList = malloc(sizeof(struct Peers));
    peerList->size = 0;
    fileList = malloc(sizeof(struct Files));
    fileList->size = 0;
}

// Fonction pour initialiser une nouvelle structure File
struct File *initFile(char *nom, char *path, char *key, int size, int piecesize)
{
    struct File *newFile = malloc(sizeof(struct File));
    if (newFile == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire pour un nouveau fichier.");
        exit(EXIT_FAILURE);
    }
    // allocate
    // Attribution des valeurs aux champs
    newFile->nom = malloc(strlen(nom) + 1);
    strcpy(newFile->nom, nom);
    newFile->path = path;
    newFile->key = malloc(strlen(key) + 1);
    strcpy(newFile->key, key);
    newFile->size = size;
    newFile->piecesize = piecesize;
    struct Peers *peerslist = malloc(sizeof(struct Peers));
    peerslist->size = 0;
    newFile->plist = peerslist;

    return newFile;
}

void freePeer(struct Peer *peer)
{
    free(peer);
}

void freePeers(struct Peers *list)
{
    for (unsigned int i = 0; i < list->size; i++)
    {
        freePeer(list->peers[i]);
    }
    free(list);
}
void freeFile(struct File *file)

{
    free(file->plist);
    free(file->nom);
    free(file);
}
void freeFiles(struct Files *list)
{
    for (unsigned int i = 0; i < list->size; i++)
    {
        freeFile(list->files[i]);
    }
    free(list);
}
void endFree()
{
    freeFiles(fileList);
    freePeers(peerList);
}
// Fonction pour ajouter un fichier à la liste de fichiers local
void addFileGeneric(struct Files *fileList1, struct File *file)
{
    if (fileList1->size >= MAX_FILES)
    {
        fprintf(stderr, "La liste de fichiers est pleine, impossible d'ajouter un nouveau fichier.\n");
        return;
    }
    // on peut faire un realloc
    fileList1->files[fileList1->size++] = file;
}

void addFile(struct File *file)
{
    addFileGeneric(fileList, file);
}
// Fonction pour trouver un fichier par sa clé
int findFileByKey(struct Files *fileList, char *key)
{
    for (unsigned int i = 0; i < fileList->size; i++)
    {
        if (strcmp(fileList->files[i]->key, key) == 0)
        {
            // printf("list file key : %s ",fileList->files[i]->key);
            return i;
        }
    }
    return -1;
}

// Fonction pour ajouter un pair à la liste des pairs
void addPeer(struct Peer *peer)
{
    if (peerList->size >= MAX_PEERS)
    {
        printf("Erreur : Nombre maximum de pairs atteint.\n");
        return;
    }

    peerList->peers[peerList->size++] = peer;
}

void addPeer_to_file(struct Peer *p, struct File *f)
{
    f->plist->peers[f->plist->size++] = p;
    printf(" size %d\n", f->plist->size);
}
// Fonction pour rechercher et retourner une liste de fichiers répondant au nom donné
struct Files *searchFilesByName(struct Files *fileList, char *filename)
{
    struct Files *resultList = malloc(sizeof(struct Files));
    if (resultList == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire pour la liste de résultats.");
        exit(EXIT_FAILURE);
    }

    resultList->size = 0;

    for (unsigned int i = 0; i < fileList->size; i++)
    {
        if (strcmp(fileList->files[i]->nom, filename) == 0)
        {

            addFileGeneric(resultList, fileList->files[i]);
        }
    }
    return resultList;
}
struct Peer *searchPeerByaddr(struct Peers *peersList, char *addr)
{

    for (unsigned int i = 0; i < peersList->size; i++)
    {
        if (strcmp(peerList->peers[i]->peer_addr, addr) == 0)
        {

            return peerList->peers[i];
        }
    }
    return NULL;
}

int is_in_filelist(char *filename)
{
    for (unsigned int i = 0; i < fileList->size; i++)
    {
        struct File *file = fileList->files[i];
        if (strcmp(file->nom, filename) == 0)
        {
            return i;
        }
    }
    return 0;
};

int is_in_leech_liste(int index, struct Peer *peer)
{
    for (int i = 0; i < peer->leechsize; i++)
    {
        if (index == peer->leechfiles[i])
            return 1;
    }
    return 0;
}

// ####################################################################
// ########             Fonctions implementation                #######
// ####################################################################

// Fonction handleLook pour récuperer par nom de fichiers seulement
void handleLook(int new_socket, char *message)
{
    char *query = strtok(message, "=[\"]");
    query = strtok(NULL, "=[\"]"); // Récupère le nom de fichier entre guillemets

    if (query == NULL)
    {
        fprintf(stderr, "Erreur: Critère non spécifié dans le message.\n");
        if (new_socket != 0)
            send(new_socket, "Erreur: Critère non spécifié dans le message.", 45, 0);
        return;
    }

    struct Files *flist = malloc(sizeof(struct Files));
    char response[1024] = "list ["; // Initialise la réponse avec "list ["

    while (query != NULL)
    {
        if (strcmp(query, "filename "))
        {
            query = strtok(NULL, "=[\"]");

            if (query == NULL)
            {
                fprintf(stderr, "Erreur: Nom de fichier non spécifié dans le message.");
                if (new_socket != 0)
                    send(new_socket, "Erreur: Nom de fichier non spécifié dans le message.", 50, 0);
                return;
            }
            else
            {
                flist = searchFilesByName(fileList, query);

                query = strtok(NULL, "=[\"]");
                if (flist->size == 0)
                {
                    strcat(response, "Aucun fichier ne correspond à votre critère");
                }

                for (unsigned int i = 0; i < flist->size; i++)
                {
                    char fileInfo[256];
                    sprintf(fileInfo, "%s %d %d %s ", flist->files[i]->nom, flist->files[i]->size, flist->files[i]->piecesize, flist->files[i]->key);
                    strcat(response, fileInfo);
                }
            }
        }
    }

    strcat(response, "]\n"); // Ajoute "]" à la fin de la réponse

    // Affiche la réponse même si new_socket == 0
    printf("> %s\n", response);

    // Envoie la réponse si new_socket != 0
    if (new_socket != 0)
    {
        int bytes_sent = send(new_socket, response, strlen(response), 0);
        if (bytes_sent < 0)
        {
            perror("Erreur lors de l'envoi de la réponse");
        }
    }
}

void handleGetFile(int new_socket, char *message)
{
    char *key = strtok(message, " "); // Récupère la clé du fichier

    if (key != NULL)
    {
        key = strtok(NULL, " ");
        printf("key %s\n", key);
    }
    else
    {
        fprintf(stderr, "Erreur: Clé de fichier non spécifiée dans le message.\n");
        if (new_socket != 0)
        {
            send(new_socket, "Erreur: Clé de fichier non spécifiée dans le message.", 52, 0);
            return;
        }
    }
    int file_index = findFileByKey(fileList, key);
    printf("file index is %d\n", file_index);
    if (file_index == -1)
    {
        printf("fichier introuvable\n");
        if (new_socket != 0)
        {
            send(new_socket, "fichier introuvable", 19, 0);
            return;
        }
    }
    struct Peers *peers = fileList->files[file_index]->plist;
    char response[1024] = "peers ";
    strcat(response, key);
    strcat(response, " [");
    printf("peers size is !!!!%d\n", peers->size);
    for (unsigned int i = 0; i < peers->size; i++)
    {
        char peerInfo[256];
        sprintf(peerInfo, "%s:%d ", peers->peers[i]->peer_addr, peers->peers[i]->port);
        printf("peer info is %s\n", peerInfo);
        strcat(response, peerInfo);
    }
    strcat(response, "]\n");

    printf("> %s\n", response);
    if (new_socket != 0)
    {
        int bytes_sent = send(new_socket, response, strlen(response), 0);
        if (bytes_sent < 0)
        {
            perror("Erreur lors de l'envoi de la réponse");
        }
    }
}

void handleAnnounce(int new_socket, char *message)
{
    char *token, *saveptr;
    struct Peer *peer = malloc(sizeof(struct Peer));
    peer->leechsize = 0;

    // Extract the port from the message
    token = strtok_r(message, " ", &saveptr);
    token = strtok_r(NULL, " ", &saveptr);
    token = strtok_r(NULL, " ", &saveptr);
    peer->port = atoi(token);
    // printf("port: %d\n", peer->port);
    // peer->peer_addr = strdup(token);
    // Extract the IP address from the socket
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);
    if (getpeername(new_socket, (struct sockaddr *)&peer_addr, &addr_len) == 0)
    {
        peer->peer_addr = strdup(inet_ntoa(peer_addr.sin_addr));
        printf("address: %s\n", peer->peer_addr);
    }
    else
    {
        perror("Error getting peer address");
        free(peer);
        return;
    }

    // Extraire les fichiers "seed"
    token = strtok_r(NULL, " ", &saveptr);
    if (strcmp(token, "seed") == 0)
    {
        token = strtok_r(NULL, "[", &saveptr);
        token = strtok_r(token, "]", &saveptr);
        char *fileinfo = strtok_r(token, " ", &saveptr);
        while (fileinfo != NULL)
        {
            char *nom = fileinfo;
            fileinfo = strtok_r(NULL, " ", &saveptr);
            int size = atoi(fileinfo);
            fileinfo = strtok_r(NULL, " ", &saveptr);
            int piecesize = atoi(fileinfo);
            fileinfo = strtok_r(NULL, " ", &saveptr);
            char *key = fileinfo;

            int x = is_in_filelist(nom);
            if (!x)
            {
                struct File *file = initFile(nom, NULL, key, size, piecesize);

                // printFile(file);
                if (file != NULL)
                {
                    addPeer_to_file(peer, file);
                    addFile(file);
                    // printFiles(fileList);

                    printf("Fichier ajouté : %s\n", file->nom);
                }
            }
            else
            {
                addPeer_to_file(peer, fileList->files[x]);
            }

            fileinfo = strtok_r(NULL, " ", &saveptr);
        }
    }

    // Extraire les fichiers "leech"
    token = strtok_r(NULL, " ", &saveptr);
    if (token != NULL && strcmp(token, "leech") == 0)
    {
        token = strtok_r(NULL, "[", &saveptr);
        token = strtok_r(token, "]", &saveptr);
        char *keyinfo = strtok_r(token, " ", &saveptr);
        int i = 0;
        while (keyinfo != NULL)
        {
            peer->leechfiles[i++] = findFileByKey(fileList, keyinfo);
            keyinfo = strtok_r(NULL, " ", &saveptr);
        }
        peer->leechsize = i;
    }

    // Ajouter le pair à la liste des pairs
    addPeer(peer);

    // Envoyer la réponse "ok" au pair
    int bytes_sent = send(new_socket, "ok\n", 3, 0);
    if (bytes_sent < 0)
    {
        perror("Erreur lors de l'envoi de la réponse");
    }
    printf("message envoyé\n");
}
void handleGetFileInfoFromTracker(int clientSocket, char *buffer)
{
    printf("about to read from client in function getinfo \n");
    printf("buffer is in get info =1=1=1=1==1%s\n", buffer);
    char *request = strtok(buffer, " ");
    if (strcmp(request, "info") == 0)
    {
        char *key = strtok(NULL, " ");
        int fileIndex = findFileByKey(fileList, key);
        if (fileIndex != -1)
        {
            struct File *file = fileList->files[fileIndex];
            char response[1024];
            snprintf(response, sizeof(response), "fileinfo %s %s %d %d\n",
                     file->key, file->nom, file->size, file->piecesize);
            printf("Sending response!!!: %s\n", response);
            write(clientSocket, response, strlen(response));
        }
        else
        {
            char *response = "filenotfound";
            write(clientSocket, response, strlen(response));
        }
    }
}
void handleUpdate(int new_socket, char *message)
{
    char *token, *saveptr;
    struct Peer *peer;
    char *addr;
    // Extract the IP address from the socket
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);
    if (getpeername(new_socket, (struct sockaddr *)&peer_addr, &addr_len) == 0)
    {
        addr = inet_ntoa(peer_addr.sin_addr);
        peer = searchPeerByaddr(peerList, addr);
    }
    else
    {
        perror("Error getting peer address");
        return;
    }

    // Extraire les fichiers "seed"
    token = strtok_r(message, " ", &saveptr);
    token = strtok_r(NULL, " ", &saveptr);
    if (strcmp(token, "seed") == 0)
    {
        token = strtok_r(NULL, "[", &saveptr);
        token = strtok_r(token, "]", &saveptr);
        char *fileinfo = strtok_r(token, " ", &saveptr);
        while (fileinfo != NULL)
        {
            char *key = fileinfo;
            int index = findFileByKey(fileList, key);
            if (index != -1)
            {
                if (searchPeerByaddr(fileList->files[index]->plist, addr) == NULL)
                {
                    addPeer_to_file(peer, fileList->files[index]);
                    printf("Fichier ajouté au peer d'addresse %s\n", peer->peer_addr);
                }
            }

            fileinfo = strtok_r(NULL, " ", &saveptr);
        }
    }

    // Extraire les fichiers "leech"
    token = strtok_r(NULL, " ", &saveptr);
    if (token != NULL && strcmp(token, "leech") == 0)
    {
        token = strtok_r(NULL, "[", &saveptr);
        token = strtok_r(token, "]", &saveptr);
        char *keyinfo = strtok_r(token, " ", &saveptr);
        while (keyinfo != NULL)
        {
            int index = findFileByKey(fileList, keyinfo);
            if (!is_in_leech_liste(index, peer))
            {
                peer->leechfiles[peer->leechsize++] = index;
                printf("Leech list modifiée pour le pair d'addresse :%s\n", peer->peer_addr);
            }

            keyinfo = strtok_r(NULL, " ", &saveptr);
        }
    }

    // Envoyer la réponse "ok" au pair
    int bytes_sent = send(new_socket, "ok\n", 3, 0);
    if (bytes_sent < 0)
    {
        perror("Erreur lors de l'envoi de la réponse");
    }
    printf("message envoyé\n");
}