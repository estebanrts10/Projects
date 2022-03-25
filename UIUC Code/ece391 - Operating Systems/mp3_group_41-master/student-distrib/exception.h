#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include "types.h"

/* Halt Flag */
// extern uint32_t volatile halt_flag = 0;

/* Exception handler functions */
extern void excep0();
extern void excep1();
extern void excep2();
extern void excep3();
extern void excep4();
extern void excep5();
extern void excep6();
extern void excep7();
extern void excep8();
extern void excep9();
extern void excep10();
extern void excep11();
extern void excep12();
extern void excep13();
extern void excep14();
extern void excep15();
extern void excep16();
extern void excep17();
extern void excep18();
extern void excep19();

extern void (*excepArr[20])(void);


#endif
