#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *taskList = NULL;

void add(char *name, int priority, int burst)
{
    Task *newTask = malloc(sizeof(Task));
    newTask->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(newTask->name, name);
    newTask->priority = priority;
    newTask->burst = newTask->remaining_burst = burst;
    insert(&taskList, newTask);
}

Task *getNextTask()
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
    while (taskList)
    {
        Task *runTask = getNextTask();
        int slice = QUANTUM < runTask->remaining_burst ? QUANTUM : runTask->remaining_burst;
        run(runTask, slice);
        runTask->remaining_burst -= slice;
        if (runTask->remaining_burst)
        {
            delete(&taskList, runTask);
            insert(&taskList, runTask);
        }
        else
        {
            delete(&taskList, runTask);
            free(runTask->name);
            free(runTask);
        }
    }
}