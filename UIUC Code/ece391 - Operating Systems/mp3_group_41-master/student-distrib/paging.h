/* paging.h - Defines used to initialize and enable paging */
#ifndef _PAGING_H
#define _PAGING_H

#define SIZE            1024
#define PDE_SPACE       1024
#define PTE_SPACE       4096
#define KB_4            4096

#define BUFF1           1
#define BUFF2           2
#define BUFF3           3

#define VIDEO_MEMORY    0xb8000
#define VID_MEM_OFFSET  184
#define ADDR_SHIFT      12
#define KERNEL_ADDR     0x400000 // 4194304 4mb
#define USER_PRGM       0x800000 // 8388608 8mb
#define PRGM_OFFSET     0x400000

#define USER_SPACE_PG   32      
#define USER_VID_PG     33 

#define BUFF_OFFSET     0x2400000 // (4mb*7)+8mb = 36mb

#include "types.h"

/* Function to initalize paging */
extern void paging_init();
extern void init_prgm();
extern void init_vid();
extern void change_vmem(int to_switch);
extern void change_vid_map_page(int to_switch);

extern volatile int vid_mem_paging;

/* Structs for pages, page_directory entries, & page directories */
typedef union page {
    uint32_t val;
    struct {
        uint32_t present    : 1;
        uint32_t r_w        : 1;
        uint32_t user_su    : 1;
        uint32_t p_wt       : 1;
        uint32_t p_cd       : 1;
        uint32_t accessed   : 1;
        uint32_t pt_written : 1;
        uint32_t p_size     : 1;
        uint32_t g_page     : 1;
        uint32_t avail      : 3;
        uint32_t addr       : 20;
    } __attribute__ ((packed));
} page;

typedef union page_direc_entry {
    uint32_t val;
    struct {
        uint32_t present    : 1;
        uint32_t r_w        : 1;
        uint32_t user_su    : 1;
        uint32_t p_wt       : 1;
        uint32_t p_cd       : 1;
        uint32_t accessed   : 1;
        uint32_t pt_written : 1;
        uint32_t p_size     : 1;
        uint32_t g_page     : 1;
        uint32_t avail      : 3;
        uint32_t addr       : 20;
    } __attribute__ ((packed));
} page_direc_entry;

typedef union page_dir {
    uint32_t val;
    struct {
        uint32_t present    : 1;
        uint32_t res0       : 2;
        uint32_t p_wt       : 1;
        uint32_t p_cd       : 1;
        uint32_t res1       : 4;
        uint32_t avail      : 3;
        uint32_t addr       : 20; 
    } __attribute__ ((packed));
} page_dir;

#endif
