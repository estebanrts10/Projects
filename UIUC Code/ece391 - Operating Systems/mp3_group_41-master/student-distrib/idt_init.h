/* idt_init.h - Defines used to initialize the IDT */
#ifndef _IDT_INIT_H
#define _IDT_INIT_H

#include "types.h"
#include "lib.h"

/* Definitions used for initializing IDT */
#define IDT_SIZE    256
#define EXCEP_RANGE 0x1F
#define PIC_RANGE   0x30
#define TIMER       0x20
#define KEYBOARD    0x21
#define RTC         0x28
#define SYS_CALL    0x80

/* IDT initializer function */
void idt_init();

#endif
