/* keyboard.h - Defines used to initialize the keyboard */
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "pagingasm.h"

/* Definitions used for keyboard initialization / handling */
#define KEYBOARD_DATA   0x60
#define KEYBOARD_STAT   0x64
#define KEYBOARD_IRQ    1
#define STAT_CHECK      0x01
#define KEYBOARD_RANGE  0x35
#define RELEASE_THRESH  0x81
#define READ_BUFF_SIZE  129
#define CHAR_ARR_SIZE   60
#define W_BUFF_SIZE     2
#define MAX_INPUT       127
#define SH_OFF_NORM     0x20
#define SH_OFF_SPEC     0x10
#define X_LIMIT         79
#define Y_LIMIT         24

//Keys
#define K_ENTER         0x1C
#define K_BACK          0x0E
#define K_SPACE         0x39
#define K_PRNT_SCRN     0xE0
#define K_CTRL_PRESS    0x1D
#define K_CTRL_RELEASE  0x9D
#define K_ALT_PRESS		0x38
#define K_ALT_RELEASE	0xB8
#define K_CAPSL         0x3A
#define K_LSHF_PRESS    0x2A
#define K_LSHF_RELEASE  0xAA
#define K_RSHF_PRESS    0x36
#define K_RSHF_RELEASE  0xB6
#define K_L             0x26
#define K_Z             0x2C
#define K_M             0x32
#define K_A             0x1E
#define K_L             0x26
#define K_Q             0x10
#define K_P             0x19
#define K_MAX           0x3B
#define K_1             0x02
#define K_2             0x03
#define K_3             0x04
#define K_4             0x05
#define F_1             0x3b
#define F_2             0x3c
#define F_3             0x3d

#include "types.h"

extern char read_buff[MAX_TERMS][READ_BUFF_SIZE];
extern unsigned int read_buff_end[MAX_TERMS];
extern volatile unsigned int enter_pressed[MAX_TERMS];

/* Function to initalize keyboard interrupts */
void keyboard_init();
void clear_buff();
void prep_ret();

/* Keyboard handler function for interrupts */
void keyboard_handler();

#endif
