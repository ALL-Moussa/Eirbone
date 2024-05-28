#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_FILES 100
#define MAX_PEERS 100

struct Peer
{
    char *peer_addr;
    int port;
    int leechfiles[MAX_FILES];
    int leechsize;
};

struct File
{
    char *nom;
    char *path;
    char *key;
    int size;
    int piecesize;
    struct Peers *plist;
};

struct Files
{
    struct File *files[MAX_FILES];
    unsigned int size;
};

struct Peers
{
    struct Peer *peers[MAX_PEERS];
    unsigned int size;
};
void init();

void addFile(struct File *file);
void initFileList();
void handleAnnounce(int new_socket, char *message);
struct File *initFile(char *nom, char *path, char *key, int size, int piecesize);
void freeFiles(struct Files *list);
// struct Files *handleLook(char *message);
void freePeers(struct Peers *list);
void freePeer(struct Peer *peer);
void endFree();
void handleGetFile(int new_socket, char *message);
void printFile(struct File *file);
void printFiles(struct Files *files);
struct Files *searchFilesByName(struct Files *fileList, char *filename);
void addPeer_to_file(struct Peer *p, struct File *f);
void addPeer(struct Peer *peer);
void handleLook(int new_socket, char *message);
void handleGetFileInfoFromTracker(int clientSocket, char *buffer);
void handleUpdate(int new_socket, char *message);