#include "idt_init.h"
#include "types.h"
#include "systemcalls.h"
#include "lib.h"
#include "exception.h"
#include "terminal.h"
#include "interrupt.h"


/* Exception Handlers
 * 
 * Exception handlers for each exception; IDT 0 - 19
 * Inputs: None
 * Outputs: Print statement based on corresponding exception
 * Side Effects: Puts program into infinite loop
 * Coverage: Load IDT
 * Files: idt_init.c
 */
void excep0() {
    printf(" Divide by 0 Error \n");   // Prints exception message
    excep_flag = 1;
    halt(0);
}
void excep1() {
    printf(" Debug Exception \n");
    excep_flag = 1;
    halt(0);
}
void excep2() {
    printf(" NMI Interrupt \n");
    excep_flag = 1;
    halt(0);
}
void excep3() {
    printf(" Breakpoint Exception \n");
    excep_flag = 1;
    halt(0);
}
void excep4() {
    printf(" Overflow Exception \n");
    excep_flag = 1;
    halt(0);
}
void excep5() {
    printf(" BOUND Range Exceeded \n");
    excep_flag = 1;
    halt(0);
}
void excep6() {
    printf(" Invalid Opcode \n");
    excep_flag = 1;
    halt(0);
}
void excep7() {
    printf(" Device Not Available \n");
    excep_flag = 1;
    halt(0);
}
void excep8() {
    printf(" Double Fault \n");
    excep_flag = 1;
    halt(0);
}
void excep9() {
    printf(" Coprocessor Segment Overrun \n");
    excep_flag = 1;
    halt(0);
}
void excep10() {
    printf(" Invalid TSS \n");
    excep_flag = 1;
    halt(0);
}
void excep11() {
    printf(" Segment Not Present \n");
    excep_flag = 1;
    halt(0);
}
void excep12() {
    printf(" Stack Fault \n");
    excep_flag = 1;
    halt(0);
}
void excep13() {
    printf(" General Protection Exception \n");
    excep_flag = 1;
    halt(0);
}
void excep14() {
    printf(" Page Fault \n");
    excep_flag = 1;
    halt(0);
}
void excep15() {
    printf("Exception 15 \n");
    excep_flag = 1;
    halt(0);
}
void excep16() {
    printf(" x87 FPU Floating-Point Error \n");
    excep_flag = 1;
    halt(0);
}
void excep17() {
    printf(" Alignment Check Exception \n");
    excep_flag = 1;
    halt(0);
}
void excep18() {
    printf(" Machine Check Exception \n");
    excep_flag = 1;
    halt(0);
}
void excep19() {
    printf(" SIMD Floating-Point Exception \n");
    excep_flag = 1;
    halt(0);
}

// Array for exception to make it easier to set idt handlers. 20 Exceptions
void (*excepArr[20])(void) =    {&excep0, &excep1,  &excep3,  &excep4,  &excep5, 
                                &excep6,  &excep7,  &excep8,  &excep9,  &excep10,
                                &excep11, &excep12, &excep13, &exception_14, &exception_15,
                                &excep16, &excep17, &excep18, &excep19};


