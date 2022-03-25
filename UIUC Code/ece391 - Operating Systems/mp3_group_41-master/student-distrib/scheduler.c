
#include "scheduler.h"
#include "systemcalls.h"
#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "paging.h"
#include "system_calls.h"
#include "x86_desc.h"

int init_counter = 0;
volatile int pit_term = 0;

/* timer_init()
 * 
 * Inputs: None
 * Outputs: None
 * Function: initializes timer
 */
void timer_init(){
    // CH0_DATA = 1193180/freq in Hz -- 59659 = 20Hz
    // write pit freq
    outb(CMD_DATA, CMD_ADDR);
    outb(CH0_L8B, CH0_ADDR);
    outb(CH0_H8B, CH0_ADDR);

    enable_irq(TIMER_IRQ);
    return;
}

/* handle_pit_init()
 * 
 * Inputs: None
 * Outputs: None
 * Function: sets up the pit & initializes shells
 */
void handle_pit_int(){
    send_eoi(TIMER_IRQ);
    cli();
    // Initializing all three shells at once
    if(init_counter < BOOTUP){
        active_term = TERM3_ID;

        // Initialize first base shell
        if(init_counter == TERM1_ID){ 
            init_counter++;
            pit_term = TERM1_ID;
            hc_change_vmem(TERM1_BUFF);
            sti();
            init_base_shell = 1;
            execute((uint8_t *)("shell"));
        }
        // Initialize second base shell
        if(init_counter == TERM2_ID){
            // Saving esp & ebp of 2nd shell
            asm volatile (  "                   \n\
                            movl   %%esp, %0    \n\
                            movl   %%ebp, %1    \n\
                            "
                            :"=r"(termArr[pit_term].curr_esp), "=r"(termArr[pit_term].curr_ebp) // output
                        );

            // Update video memory & counters
            hc_change_vmem(TERM2_BUFF);
            init_counter++;
            pit_term = TERM2_ID;
            sti();
            init_base_shell = 1;
            execute((uint8_t *)("shell"));
        }
        // Initialize third base shell
        if(init_counter == TERM3_ID){
            // Saving esp & ebp of 3rd shell
            asm volatile (  "                   \n\
                            movl   %%esp, %0    \n\
                            movl   %%ebp, %1    \n\
                            "
                            :"=r"(termArr[pit_term].curr_esp), "=r"(termArr[pit_term].curr_ebp) // output
                        );

            // Update video memory & counters
            hc_change_vmem(VIDEO);
            init_counter++;
            pit_term = TERM3_ID;
            sti();
            init_base_shell = 1;
            execute((uint8_t *)("shell"));
        }
    }
    else{
        // Setting current ebp & esp
        asm volatile (  "                   \n\
                        movl   %%esp, %0    \n\
                        movl   %%ebp, %1    \n\
                        "
                        :"=r"(termArr[pit_term].curr_esp), "=r"(termArr[pit_term].curr_ebp) // output
                    );
        // *task_proc_ctrl->vidmap_addr = 0;

        // Incrementing pit terminal number
        pit_term++;
        if(pit_term > TERM3_ID){
            pit_term = TERM1_ID;
        }

        // Updating video memory 
        if(pit_term == active_term){
            hc_change_vmem(SCREEN);
            change_vid_map_page(SCREEN);
        }
        else{
            hc_change_vmem(pit_term + 1);
            change_vid_map_page(pit_term + 1);
        }

        // Setting PCB location
        task_proc_ctrl = (proc_ctrl_t *)(MB_8 - (KB_8 * termArr[pit_term].curr_proc_id));
        tss.esp0 = (MB_8 - (KB_8 * termArr[pit_term].curr_proc_id) + KB_8 - ESP_OFFSET); 

        // Initialize page of current process & flushing TLB
        init_prgm(termArr[pit_term].curr_proc_id);
        flush_tlb();

        // if(task_proc_ctrl->vidmap_addr != 0){
        //     vidmap(task_proc_ctrl->vidmap_addr);
        // }

        // Updating cursor information
        changePos(1, 0, termArr[pit_term].curs_x, termArr[pit_term].curs_y);
        if(pit_term == active_term){
            update_cursor(changePos(0, 0, 0, 0), changePos(0, 1, 0, 0));
        }

        sti();

        // Setting esp & ebp from current esp / ebp
        asm volatile (  "                   \n\
                        movl   %0, %%esp    \n\
                        movl   %1, %%ebp    \n\
                        "
                        : // output
                        : "r" (termArr[pit_term].curr_esp), "r" (termArr[pit_term].curr_ebp)
                    );
    }
    return;
}
