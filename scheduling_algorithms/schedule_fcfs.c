#include <string.h>
#include <stdlib.h>
#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *taskList = NULL;

void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(newTask->name, name);
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&taskList, newTask);
}

Task *getTaskTail() {
    struct node *tail = taskList;
    while(tail->next) {
        tail = tail->next;
    }
    return tail->task;
}

void schedule() {
    while (taskList) {
        Task *runTask = getTaskTail();
        run(runTask, runTask->burst);
        delete(&taskList, runTask);
        free(runTask->name);
        free(runTask);
    }
}
