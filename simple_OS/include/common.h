#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

#define ADDRESS_SIZE 20
#define OFFSET_LEN 10
#define SEGMENT_LEN 5
#define PAGE_LEN 5

#define NUM_PAGES (1 << (ADDRESS_SIZE - OFFSET_LEN))
#define PAGE_SIZE (1 << OFFSET_LEN)

typedef char BYTE;
typedef uint32_t addr_t;

enum ins_opcode_t
{
    CALC,  // Just perform calculation, only use CPU
    ALLOC, // Allocate memory
    FREE,  // Deallocate a memory block
    READ,  // Read data from a byte on memory
    WRITE  // Write data to a byte on memory
};

/* instructions executed by the CPU */
struct inst_t
{
    enum ins_opcode_t opcode;
    uint32_t arg_0; // Argument lists for instructions
    uint32_t arg_1;
    uint32_t arg_2;
};

struct code_seg_t
{
    struct inst_t *text;
    uint32_t size;
};

struct page_table_t
{
    /* A row in the page table of the second layer */
    struct
    {
        addr_t v_index; // The index of virtual address
        addr_t p_index; // The index of physical address
    } table[1 << PAGE_LEN];
    int size;
};

struct seg_table_t
{
    /* Translation table for the first layer*/
    struct
    {
        addr_t v_index; // Virtual index
        struct page_table_t *pages;
    } table[1 << SEGMENT_LEN];
    int size;
};

struct pcb_t
{
    uint32_t pid;
    uint32_t priority;
    struct code_seg_t *code;
    addr_t regs[10];
    uint32_t pc;
    struct seg_table_t *seg_table;
    uint32_t bp;
};

#endif // _COMMON_H_