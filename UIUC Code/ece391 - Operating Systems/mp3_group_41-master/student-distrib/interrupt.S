#define     ASM     1
#include    "interrupt.h"
#include    "x86_desc.h"

.globl   rtc_helper, keyboard_helper, timer_helper, exception_14, exception_15

/* rtc_helper
 * Inputs: None
 * Outputs: None
 * Function: assembly rtc helper function
 */
rtc_helper:                     
    pushal
    pushfl
    call handle_rtc_int
    popfl
    popal
    iret

/* timer_helper
 * Inputs: None
 * Outputs: None
 * Function: assembly timer helper function
 */
timer_helper:                     
    pushal
    pushfl
    call handle_pit_int
    popfl
    popal
    iret

/* keyboard_helper
 * Inputs: None
 * Outputs: None
 * Function: assembly keyboard helper function
 */
keyboard_helper:                     
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

/* exception_14
 * Inputs: None
 * Outputs: None
 * Function: exception 14 helper function
 */
exception_14:
    pushfl
    popfl
    call excep14

/* exception_15
 * Inputs: None
 * Outputs: None
 * Function: exception 15 helper function
 */
exception_15:
    pushfl
    popfl
    call excep15
