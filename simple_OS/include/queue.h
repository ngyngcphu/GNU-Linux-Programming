#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "common.h"

#define MAX_QUEUE_SIZE 10

struct queue_t
{
    struct pcb_t *proc[MAX_QUEUE_SIZE];
    int size;
};

void enqueue(struct queue_t *q, struct pcb_t *proc);

struct pcb_t *dequeue(struct queue_t *q);

int empty(struct queue_t *q);

#endif // _QUEUE_H_