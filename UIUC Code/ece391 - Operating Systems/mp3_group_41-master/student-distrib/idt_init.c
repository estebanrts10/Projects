/* i8259.c - Functions to initialize the IDT */

#include "idt_init.h"
#include "keyboard.h"
#include "x86_desc.h"
#include "interrupt.h"
#include "lib.h"
#include "RTC.h"
#include "exception.h"
#include "scheduler.h"
#include "systemcalls.h"
#include "system_calls.h"

unsigned char buff[128];
int ctr = 0;

// Initialize the IDT
/*
*   void idt_init()
*   Inputs: none
*   Return Value: none
*	Function: initialize the Interrupt Descriptor table
*/
void idt_init() {
    int i;
    for (i = 0; i < IDT_SIZE; i++) {
        if (i <= EXCEP_RANGE) {
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4    = 0;
            idt[i].reserved3    = 0;
            idt[i].reserved2    = 1;
            idt[i].reserved1    = 1;
            idt[i].size         = 1;      // 1 = 32 bits, 0 = 16 bits
            idt[i].reserved0    = 0;
            idt[i].dpl          = 0;      // 0 to 3 based on priority
            idt[i].present      = 1;      // 1 if active, 0 if not used
            SET_IDT_ENTRY(idt[i], excepArr[i]);
        }
        else if((EXCEP_RANGE < i) && (i < PIC_RANGE)){
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4    = 0;
            idt[i].reserved3    = 0;
            idt[i].reserved2    = 1;
            idt[i].reserved1    = 1;
            idt[i].size         = 1;      // 1 = 32 bits, 0 = 16 bits
            idt[i].reserved0    = 0;
            idt[i].dpl          = 0;      // 0 to 3 based on priority; system calls should have DPL set to 3
            idt[i].present      = 1;      // 1 if active, 0 if not used

            // Interrupt entry for the timer and keyboard handler
            if (i == TIMER){
                SET_IDT_ENTRY(idt[i], timer_helper); 
            }
            if (i == KEYBOARD){
                SET_IDT_ENTRY(idt[i], keyboard_helper); 
            }
            // Interrupt entry for the rtc helper function
            if (i == RTC){
                SET_IDT_ENTRY(idt[i], rtc_helper);
            }
            
        }
        // Set IDT values for system calls
        else if(i == SYS_CALL){
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4    = 0;
            idt[i].reserved3    = 0;
            idt[i].reserved2    = 1;
            idt[i].reserved1    = 1;
            idt[i].size         = 1;      // 1 = 32 bits, 0 = 16 bits
            idt[i].reserved0    = 0;
            idt[i].dpl          = 3;      // 0 to 3 based on priority; system calls should have DPL set to 3
            idt[i].present      = 1;      // 1 if active, 0 if not used

            // Interrupt entry for system calls
            if (i == SYS_CALL){
                SET_IDT_ENTRY(idt[i], generic_linkage_helper);
            }
        }
        else{
            idt[i].present = 0;
        }
    }
}
unsigned char * get_buff(void){
  return buff;
}
