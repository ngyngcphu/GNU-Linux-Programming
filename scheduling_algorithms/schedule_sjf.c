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
    newTask->burst = burst;
    insert(&taskList, newTask);
}

Task *getNextTask()
{
    Task *shortestTask = taskList->task;
    struct node *temp = taskList;
    while (temp)
    {
        if (temp->task->burst <= shortestTask->burst)
        {
            shortestTask = temp->task;
        }
        temp = temp->next;
    }
    return shortestTask;
}

void schedule()
{
    while (taskList)
    {
        Task *runTask = getNextTask();
        run(runTask, runTask->burst);
        delete (&taskList, runTask);
        free(runTask->name);
        free(runTask);
    }
}