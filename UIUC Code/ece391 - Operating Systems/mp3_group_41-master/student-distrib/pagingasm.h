#ifndef _PAGINGASM_H
#define _PAGINGASM_H

#include "types.h"
#ifndef ASM

/* Paging helper functions */
extern void paging_helper(unsigned int arg);

extern void helper_long(uint32_t test_l);
extern void helper_word(uint16_t test_w);
extern void helper_byte(uint8_t test_b);

/* Exception 13 assembly helper function */
extern void exception13();

#endif
#endif
