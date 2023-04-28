#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
    return q->size == 0;
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
    if (q->size == MAX_QUEUE_SIZE)
    {
        printf("Queue is full !\n");
        return;
    }
    q->proc[q->size] = proc;
    q->size++;
}

struct pcb_t *dequeue(struct queue_t *q)
{
    if (q->size == 0)
    {
        return NULL;
    }
    struct pcb_t *temp = q->proc[0];
    int index = 0;
    int length = q->size;
    for (int i = 1; i < length; ++i)
    {
        if (temp->priority < q->proc[i]->priority)
        {
            temp = q->proc[i];
            index = i;
        }
    }
    q->proc[index] = q->proc[length - 1];
    q->proc[length - 1] = NULL;
    q->size--;

    return temp;
}