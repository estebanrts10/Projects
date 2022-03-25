#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "types.h"
#ifndef ASM

/* Handler for RTC interrupts */
extern void rtc_helper();
extern void timer_helper();
extern void keyboard_helper();
extern void exception_15();
extern void exception_14();

#endif
#endif
