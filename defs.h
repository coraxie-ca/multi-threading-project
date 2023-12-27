#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000    //5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define FEAR_INC        1
#define BOREDOM_INC     1
#define LOGGING         C_TRUE

typedef enum EvidenceType       EvidenceType;
typedef enum GhostClass         GhostClass;
typedef enum LoggerDetails      LoggerType;

typedef struct Room             RoomType;
typedef struct RoomNode         RoomNodeType;
typedef struct RoomList         RoomListType;
typedef struct EvidenceNode     EvidenceNodeType;
typedef struct EvidenceList     EvidenceListType;
typedef struct Hunter           HunterType;
typedef struct HunterArray      HunterArrayType;
typedef struct Ghost            GhostType;
typedef struct House            HouseType;
typedef struct HunterThreadArg  HunterThreadArgType;
typedef struct GhostThreadArg   GhostThreadArgType;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

struct RoomNode {
    RoomType        *data;
    struct RoomNode *next;
};

struct RoomList {
    RoomNodeType    *head;
    RoomNodeType    *tail;
    int             size;
};

struct EvidenceNode {
    EvidenceType            *data;
    struct EvidenceNode     *next;
};

struct EvidenceList {
    EvidenceNodeType    *head;
    EvidenceNodeType    *tail;
    sem_t               mutex;
};

struct Hunter {
    RoomType            *room; 
    EvidenceType        equipment;
    char                name[MAX_STR];
    EvidenceListType    *evidenceSharedList;
    int                 fear;
    int                 boredom;
};

struct HunterArray {
    HunterType *hunter[NUM_HUNTERS];
    int         size;
};

struct Ghost {
    GhostClass      ghostClass;
    RoomType        *room;
    int             boreTimer;
};

struct Room {
    char                name[MAX_STR];
    RoomListType        roomList;
    EvidenceListType    evidenceList;
    HunterArrayType     hunters;
    GhostType           *ghost;
    sem_t               mutex;
};

struct House {
    GhostType           *ghost;
    HunterArrayType     hunters;
    RoomListType        rooms;
    EvidenceListType    evidence;
    EvidenceType        equipmentSet[EV_COUNT];
    int                 ghostHasEnd;
};

struct HunterThreadArg {
    HouseType   *house;
    HunterType  *hunter;
};

struct GhostThreadArg {
    HouseType   *house;
    GhostType   *ghost;
};


// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

// room-related functions
void initRoomList(RoomListType *roomList);
RoomType* createRoom(char *name);
void addRoom(RoomListType *roomList, RoomType *room);
void connectRooms(RoomType *room1, RoomType *room2);
void cleanupRoomList(RoomListType *roomList);
void cleanupRoom(RoomType *room);

// evidence-related functions
void initEvidenceList(EvidenceListType *evList);
void addEvidence(EvidenceType *ev, EvidenceListType *evList);
void cleanupEvidenceList(EvidenceListType *evList);

// ghost-related functions
void initGhost(GhostType **ghost, HouseType *house);
void ghostLeaveEvidence(GhostType *ghost);
void ghostMove(GhostType *ghost);
void* ghostThread(void* tGhost);

// hunter-related functions
void initHunterArr(HunterArrayType *hunterArr);
void addHunter(HunterType *hunter, HunterArrayType *arr);
void initHunter(char *name, HunterType **hunter, HouseType *house, int equipmentNo);
void cleanupHunterArr(HunterArrayType *hunterArr);
void hunterCondition(HunterType *hunter);
void hunterLeaveRoom(HunterType *hunter);
void hunterMove(HunterType *hunter);
void hunterCollect(HunterType *hunter, HouseType *house);
void hunterReveiw(HunterType *hunter, HouseType *house);
void* hunterThread(void* arg);

// house-related functions
void initHouse(HouseType *house);
void cleanupHouse(HouseType *house);
void populateRooms(HouseType* house);