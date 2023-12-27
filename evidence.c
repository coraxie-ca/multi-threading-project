#include "defs.h"

/*
*/
void initEvidenceList(EvidenceListType *evList) {
    evList->head = NULL;
    evList->tail = NULL;
    sem_init(&evList->mutex, 0, 1);
}

/*
*/
void addEvidence(EvidenceType *ev, EvidenceListType *evList) {
    EvidenceNodeType *evNode;
    evNode = (EvidenceNodeType *)malloc(sizeof(EvidenceNodeType));
    evNode->data = ev;
    evNode->next = NULL;

    if (evList->head == NULL) {
        evList->head = evNode;
        evList->tail = evNode;
    } else {
        evList->tail->next = evNode;
        evList->tail = evNode;
    }
}

/*
*/
void cleanupEvidenceList(EvidenceListType *evList) {
    EvidenceNodeType *curNode;
    EvidenceNodeType *nextNode;
    curNode = evList->head;

    while (curNode != NULL) {
        nextNode = curNode->next;
        free(curNode);
        curNode = nextNode;
    }
}