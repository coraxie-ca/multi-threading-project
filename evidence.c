#include "defs.h"

/*
  Function: initEvidenceList
  Purpose:  initialize the evidence list by setting the head and tail to NULL, init mutex
  Params:  
    Input/Output: EvidenceListType *evList - the evidence list to be initialized
*/
void initEvidenceList(EvidenceListType *evList) {
    evList->head = NULL;
    evList->tail = NULL;
    sem_init(&evList->mutex, 0, 1);
}

/*
  Function: addEvidence
  Purpose:  append an evidence to the end of the evidence list
  Params:  
    Input/Output:   EvidenceListType *evList - the evidence list to append to
    Input:          EvidenceType *ev - the evidence to append
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
  Function: cleanupEvidenceList
  Purpose:  deallocate momery of all the nodes in the evidence list
  Params:  
    Input/Output: EvidenceListType *evList - the evidence list to free nodes in
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
