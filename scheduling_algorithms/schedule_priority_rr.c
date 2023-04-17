#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *taskList[MAX_PRIORITY];

void add(char *name, int priority, int burst)
{
    Task *newTask = malloc(sizeof(Task));
    newTask->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(newTask->name, name);
    newTask->priority = priority;
    newTask->burst = newTask->remaining_burst = burst;
    insert(&taskList[priority], newTask);
}

Task *getNextTask(struct node *taskList)
{
    struct node *lastTask = taskList;
    while (lastTask->next)
    {
        lastTask = lastTask->next;
    }
    return lastTask->task;
}

void schedule()
{
    for (int i = MAX_PRIORITY; i >= MIN_PRIORITY; --i)
    {
        while (taskList[i])
        {
            Task *runTask = getNextTask(taskList[i]);
            int slice = QUANTUM < runTask->remaining_burst ? QUANTUM : runTask->remaining_burst;
            run(runTask, slice);
            runTask->remaining_burst -= slice;
            if (runTask->remaining_burst)
            {
                delete(&taskList[i], runTask);
                insert(&taskList[i], runTask);
            }
            else
            {
                delete(&taskList[i], runTask);
                free(runTask->name);
                free(runTask);
            }
        }
    }
}
