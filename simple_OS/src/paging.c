#include <stdio.h>
#include <stdlib.h>
#include "loader.h"
#include "cpu.h"
#include "mem.h"

int main(int argc, char **argv)
{
    if (argv < 2)
    {
        printf("Cannot find input process\n");
        exit(1);
    }
    struct pcb_t *proc = load(argv[1]);
    for (unsigned int i = 0; i < proc->code->size; ++i)
    {
        run(proc);
    }
    dump();
    return 0;
}