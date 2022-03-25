#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "types.h"
#ifndef ASM

/* Helper functions for system calls */
extern void halt_helper ();

extern void execute_helper ();

extern void read_helper (); 

extern void write_helper ();

extern void open_helper ();

extern void close_helper ();

extern void getargs_helper ();

extern void vidmap_helper ();

extern void set_handler_helper ();

extern void sigreturn_helper ();

/* Generic assembly linkage helper function */
extern void generic_linkage_helper ();

extern void base_shell_halt();

extern void execute_user_iret (uint32_t ss_reg, uint32_t file_esp, uint32_t cs_reg, uint32_t file_eip);

extern void finish_halt(uint32_t status_32, uint32_t esp, uint32_t ebp, uint32_t flag, uint32_t base_fl);

/* Function to flush TLB */
extern void flush_tlb();

/* Function for invalid calls */
extern void invalid_call ();

#endif
#endif
