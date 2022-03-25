/* paging.h - Defines used to initialize and enable paging */
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "types.h"

#define CH0_ADDR    0x40
#define CH0_L8B     0x0B
#define CH0_H8B     0xE9     
#define CMD_ADDR    0x43
#define CMD_DATA    0x36

#define TIMER_IRQ   0
#define BOOTUP      3
#define ESP_OFFSET  4

/* Initializes timer */
extern void timer_init();

/* Sets up pit & multi shells */
extern void handle_pit_int();

extern volatile int pit_term;
#endif
