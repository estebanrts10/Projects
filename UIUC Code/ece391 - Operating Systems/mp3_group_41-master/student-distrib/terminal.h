/* paging.h - Defines used to initialize and enable paging */
#ifndef _TERMINAL_H
#define _TERMINAL_H

#define SIZE            1024
#define NULL_C          '\0'
#define MAX_TERMS       3

#define TERM1_ID        0
#define TERM2_ID        1
#define TERM3_ID        2

#define KB4             4096
#define BUFF1_ADDR      0x1000
#define BUFF2_ADDR      0x2000
#define BUFF3_ADDR      0x3000

#include "types.h"

/* Terminal struct */
typedef struct term_struct {
    int         curs_x;
    int         curs_y;
    uint32_t    term_id;
    uint32_t    buf_addr;
    uint32_t    curr_ebp;
    uint32_t    curr_esp;
    uint32_t    base_ebp;
    uint32_t    base_esp;       
    uint32_t    curr_proc_id;
} term_struct_t;

/* Function to initalize paging */
int terminal_write(int32_t fd, void* buf, int32_t nbytes);
int terminal_read(int32_t fd, void* buf, int32_t nbytes);
int terminal_close();
int terminal_open();

extern int switch_terms(int term_num);
extern int upd_term_curs(int term_num, int x, int y);
extern int update_term_proc(int term_num, uint32_t proc_id, uint32_t proc_ebp, uint32_t proc_esp);
extern volatile int active_term;
extern volatile term_struct_t termArr[MAX_TERMS];

// shell      // shell       // shell

// shell      // shell       // shell
// shell                       // pr2

// shell      // shell       // shell
// shell                       // pr2
// pr3                       

// pr3->shell->pr2

#endif
