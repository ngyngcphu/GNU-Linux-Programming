#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mem.h"

static BYTE _ram[RAM_SIZE];

static struct
{
    uint32_t proc;
    int index;
    int next;
} _mem_stat[NUM_PAGES];

static pthread_mutex_t mem_lock;

void init_mem(void)
{
    memset(_mem_stat, 0, sizeof(*_mem_stat) * NUM_PAGES);
    memset(_ram, 0, sizeof(BYTE) * RAM_SIZE);
    pthread_mutex_init(&mem_lock, NULL);
}

/* get offset of the virtual address */
static addr_t get_offset(addr_t addr)
{
    return addr & ~((~0U) << OFFSET_LEN);
}

/* get the first layer index */
static addr_t get_first_lv(addr_t addr)
{
    return addr >> (OFFSET_LEN + PAGE_LEN);
}

/* get the second layer index */
static addr_t get_second_lv(addr_t addr)
{
    return (addr >> OFFSET_LEN) - (get_first_lv(addr) << PAGE_LEN);
}

/* Search for page table table from the a segment table */
static struct page_table_t *get_page_table(addr_t index, struct seg_table_t *seg_table)
{
    int size = seg_table->size;
    for (int i = 0; i < size; ++i)
    {
        if (seg_table->table[i].v_index == index)
        {
            return seg_table->table[i].pages;
        }
    }
    return NULL;
}

static int translate(addr_t virtual_addr, addr_t *physical_addr, struct pcb_t *proc)
{
    addr_t offset = get_offset(virtual_addr);
    addr_t first_lv = get_first_lv(virtual_addr);
    addr_t second_lv = get_second_lv(virtual_addr);

    struct page_table_t *page_table = get_page_table(first_lv, proc->seg_table);
    if (page_table == NULL)
    {
        return 0;
    }
    for (int i = 0; i < page_table->size; ++i)
    {
        if (page_table->table[i].v_index == second_lv)
        {
            if (physical_addr)
            {
                *physical_addr = (page_table->table[i].p_index << OFFSET_LEN) + offset;
                return 1;
            }
        }
    }
    return 0;
}

addr_t alloc_mem(uint32_t size, struct pcb_t *proc)
{
    pthread_mutex_lock(&mem_lock);

    addr_t ret_mem = 0;
    uint32_t num_pages = (size % PAGE_SIZE) ? (size / PAGE_SIZE + 1) : (size / PAGE_SIZE);
    int mem_avail = 0;

    uint32_t num_free_pages = 0;
    for (int i = 0; i < NUM_PAGES; ++i)
    {
        if (_mem_stat[i].proc == 0)
        {
            num_free_pages++;
        }
    }
    if (num_free_pages >= num_pages)
    {
        if (num_pages * PAGE_SIZE + proc->bp <= RAM_SIZE)
        {
            mem_avail = 1;
        }
    }
    if (mem_avail)
    {
        ret_mem = proc->bp;
        proc->bp += num_pages * PAGE_SIZE;
        int i = 0;    // Index of the page which will be allocated
        int idx = 0;  // Iterator for _mem_stats
        int prev = 0; // Index of previous frame
        while (i < num_pages && idx < NUM_PAGES)
        {
            if (_mem_stat[idx].proc == 0)
            {
                _mem_stat[idx].proc = proc->id;
                _mem_stat[idx].index = i;
                _mem_stat[idx].next = -1;
                if (i > 0)
                {
                    _mem_stat[prev].next = idx;
                }
                uint32_t virtual_addr = ret_mem + i * PAGE_SIZE;
                uint32_t seg_idx = get_first_lv(virtual_addr);
                uint32_t page_table_idx = get_second_lv(virtual_addr);

                struct page_table_t *pages = get_page_table(seg_idx, proc->seg_table);
                if (!pages)
                {
                    proc->seg_table->table[proc->seg_table->size].v_index = seg_idx;
                    pages = (struct page_table_t *)malloc(sizeof(struct page_table_t));
                    pages->size = 0;
                    proc->seg_table->table[proc->seg_table->size].pages = pages;
                    proc->seg_table->size++;
                }
                pages->table[pages->size].v_index = page_table_idx;
                pages->table[pages->size].p_index = idx;
                pages->size++;

                prev = idx;
                ++i;
            }
            ++idx;
        }
    }
    pthread_mutex_unlock(&mem_lock);
    return ret_mem;
}

int free_mem(addr_t address, struct pcb_t *proc)
{
    pthread_mutex_lock(&mem_lock);
    addr_t physical_addr;
    if (translate(address, &physical_addr, proc) == 0)
    {
        pthread_mutex_unlock(&mem_lock);
        return 1;
    }
    addr_t p_index = physical_addr >> OFFSET_LEN;

    addr_t virtual_addr = address;
    while (p_index != -1)
    {
        int temp = _mem_stat[p_index].next;
        _mem_stat[p_index].proc = 0;
        _mem_stat[p_index].index = -1;
        _mem_stat[p_index].next = -1;
        uint32_t seg_idx = get_first_lv(virtual_addr);
        uint32_t page_table_idx = get_second_lv(virtual_addr);
        struct page_table_t *page = get_page_table(seg_idx, proc->seg_table);

        int i = 0;
        while (pages->table[i].v_index != page_table_idx)
        {
            ++i;
        }
        pages->table[i].p_index = pages->table[pages->size - 1].p_index;
        pages->table[i].v_index = pages->table[pages->size - 1].v_index;

        pages->size--;

        if (pages->size == 0)
        {
            for (int i = 0; i < proc->seg_table->size; ++i)
            {
                if (proc->seg_table->table[i].v_index == seg_idx)
                {
                    break;
                }
            }
            proc->seg_table->table[i] = proc->seg_table->table[proc->seg_table->size - 1];
            proc->seg_table->size--;
            free(pages);
        }
        virtual_addr += PAGE_SIZE;
        p_index = temp;
    }

    pthread_mutex_unlock(&mem_lock);
    return 0;
}

int read_mem(addr_t address, struct pcb_t *proc, BYTE *data)
{
    addr_t physical_addr;
    if (translate(address, &physical_addr, proc))
    {
        *data = _ram[physical_addr];
        return 0;
    }
    else
    {
        return 1;
    }
}

int write_mem(addr_t address, struct pcb_t *proc, BYTE data)
{
    addr_t physical_addr;
    if (translate(address, &physical_addr, proc))
    {
        _ram[physical_addr] = data;
        return 0;
    }
    else
    {
        return 1;
    }
}

void dump(void)
{
    for (int i = 0; i < NUM_PAGES; ++i)
    {
        if (_mem_stat[i].proc != 0)
        {
            printf("%03d: ", i);
            printf("%05x-%05x - PID: %02d (idx %03d, nxt: %03d)\n, 
                i << OFFSET_LEN,
                ((i + 1) << OFFSET_LEN) - 1,
                _mem_stat[i].proc,
                _mem_stat[i].index,
                _mem_stat[i].next
            ");
            for (int j = i << OFFSET_LEN; j < ((i+1) << OFFSET_LEN) - 1; ++j)
            {
                if (_ram[j] != 0)
                {
                    printf("\t%05x: %02x\n", j, _ram[j]);
                }
            }
        }
    }
}