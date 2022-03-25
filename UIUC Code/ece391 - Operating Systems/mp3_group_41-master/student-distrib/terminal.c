/* terminal.c - Functions to initialize and enable the terminal*/

#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "scheduler.h"
#include "paging.h"
#include "system_calls.h"
#include "systemcalls.h"
#include "x86_desc.h"

volatile term_struct_t termArr[MAX_TERMS];
volatile int active_term = 0;
int init[MAX_TERMS] = {1, 0, 0};

/* upd_term_curs()
 * 
 * Inputs: None
 * Outputs: None
 * Function: returns active term num
 */
int upd_term_curs(int term_num, int x, int y){
    if(term_num < 0){
        return -1;
    }
    if(term_num > (MAX_TERMS-1)){
        return -1;
    }
    termArr[term_num].curs_x = x;
    termArr[term_num].curs_y = y;
    
    return 0;
}

/* update_term_proc()
 * 
 * Inputs: None
 * Outputs: None
 * Function: updates terminal process info
 */
int update_term_proc(int term_num, uint32_t proc_id, uint32_t proc_ebp, uint32_t proc_esp){
    if(term_num < 0){
        return -1;
    }
    if(term_num > (MAX_TERMS-1)){
        return -1;
    }

    termArr[term_num].curr_proc_id = proc_id;
    termArr[term_num].curr_ebp = proc_ebp;
    termArr[term_num].curr_esp = proc_esp;

    return 0;
}


/* switch_term()
 * 
 * Inputs: None
 * Outputs: None
 * Function: switches term
 */
int switch_terms(int term_num){
    void *save, *restore;
    int x, y, curr_paging;

    // Checks to see if terminal number argument is valid
    if(term_num < 0){
        return -1;
    }
    if(term_num > (MAX_TERMS-1)){
        return -1;
    }
    cli();

    // Saving video memory buffer addresses & cursor positions
    save = (void *)termArr[active_term].buf_addr;
    restore =  (void *)termArr[term_num].buf_addr;
    x = termArr[term_num].curs_x;
    y = termArr[term_num].curs_y;
    
    // Set paging variables
    curr_paging = curr_vmem;

    // Save current video memory & switch video memory to diff terminal
    switch_to_screen();
    memcpy(save, (void *)VIDEO, KB4);
    memcpy((void *)VIDEO, restore, KB4);

    // Set active terminal to switched terminal
    active_term = term_num;

    // Updating cursor location
    changePos(1, 0, x, y);
    update_cursor(x, y);

    // Restore video memory to current page
    restore_pit_buff(curr_paging);
    sti();
    
    return 0;
}

/* terminal_open()
 * 
 * Inputs: None
 * Outputs: None
 * Function: initializes terminal
 */
int terminal_open(){
    int i;
    termArr[TERM1_ID].term_id = TERM1_ID;
    termArr[TERM2_ID].term_id = TERM2_ID;
    termArr[TERM3_ID].term_id = TERM3_ID;

    // Sets initial cursor location
    termArr[TERM1_ID].curs_x = 0;
    termArr[TERM1_ID].curs_y = 0;
    termArr[TERM2_ID].curs_x = 0;
    termArr[TERM2_ID].curs_y = 0;
    termArr[TERM3_ID].curs_x = 0;
    termArr[TERM3_ID].curs_y = 0;

    // Sets initial ebp & esp to 0
    termArr[TERM1_ID].curr_ebp= 0;
    termArr[TERM1_ID].curr_esp = 0;

    termArr[TERM2_ID].curr_ebp = 0;
    termArr[TERM2_ID].curr_esp= 0;

    termArr[TERM3_ID].curr_ebp = 0;
    termArr[TERM3_ID].curr_esp = 0;

    // Sets buffer addresses for 3 terminals
    termArr[TERM1_ID].buf_addr = BUFF1_ADDR;
    termArr[TERM2_ID].buf_addr = BUFF2_ADDR;
    termArr[TERM3_ID].buf_addr = BUFF3_ADDR;

    // Making terminals clear
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(BUFF1_ADDR + (i << 1)) = ' ';
        *(uint8_t *)(BUFF1_ADDR + (i << 1) + 1) = ATTRIB;
    }
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(BUFF2_ADDR + (i << 1)) = ' ';
        *(uint8_t *)(BUFF2_ADDR + (i << 1) + 1) = ATTRIB;
    }
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(BUFF3_ADDR + (i << 1)) = ' ';
        *(uint8_t *)(BUFF3_ADDR + (i << 1) + 1) = ATTRIB;
    }
    return 0;
}

/* terminal_close()
 * 
 * Inputs: None
 * Outputs: None
 * Function: closes terminal
 */
int terminal_close(){

    return 0;
}

/* terminal_read()
 * 
 * Inputs: file descriptor, buffer, number of bytes
 * Outputs: -1 if error, else number of bytes read
 * Function: read terminal
 */
int terminal_read(int32_t fd, void* buf, int32_t nbytes){
    int i; 
    char* buffer;

    // Waits for enter to be pressed
    while (enter_pressed[pit_term] == 0){}
    
    // Checks if buffer is empty
    if(buf == 0){
        return -1;
    }

    buffer = (char *)buf;

    // Loops through bytes inputted for end cases
    for(i=0; i<nbytes; i++){
        // If new line character
        if(read_buff[pit_term][i] == '\n'){
            buffer[i] = read_buff[pit_term][i];
            clear_buff();
            read_buff_end[pit_term] = 0;
            enter_pressed[pit_term] = 0;
            return i+1;
        }
        // If reaches end of buffer
        else if(i >= read_buff_end[pit_term]){
            clear_buff();
            read_buff_end[pit_term] = 0;
            enter_pressed[pit_term] = 0;
            return i;
        }
        // If input is greater than buffer size
        else if(i >= READ_BUFF_SIZE){
            buffer[i] = read_buff[pit_term][i];
            clear_buff();
            read_buff_end[pit_term] = 0;
            enter_pressed[pit_term] = 0;
            return i+1;
        }
        // General case to read a byte
        else{
            buffer[i] = read_buff[pit_term][i];
            read_buff[pit_term][i] = '\0';
        }
    }
    clear_buff();
    read_buff_end[pit_term] = 0;
    enter_pressed[pit_term] = 0;
    return i;
}

/* terminal_write()
 * 
 * Inputs: file descriptor, buffer, number of bytes
 * Outputs: -1 if error, else number of bytes written
 * Function: writes to terminal
 */
int terminal_write(int32_t fd, void* buf, int32_t nbytes){
    int i, x, y;
    char* buffer;

    // Checks if buffer is null
    if(buf == 0){
        return -1;
    }
    buffer = (char *)buf;

    // Checks if pit terminal == active terminal for scheduling
    cli();
    if(pit_term == active_term){
        hc_change_vmem(SCREEN);       // change lib.c video memory pointer to point to video memory
    }
    else{
        hc_change_vmem(pit_term + 1); // change lib.c video memory pointer to point to buffer
    }
    // Set screen x & y to current terminal location
    changePos(1, 0, termArr[pit_term].curs_x, termArr[pit_term].curs_y);

    // Loops through number of bytes from user & writes to screen
    for(i=0; i < nbytes; i++){

        // Sets x & y to current screen position
        x = changePos(0, 0, 0, 0);
        y = changePos(0, 1, 0, 0);

        // Checks if cursor is at edge of screen
        if(x == X_LIMIT){
            if(y == Y_LIMIT){       // Bottom most corner
                shift_vid_mem();    // Moves video memory up one line
                // putc(buffer[i]);
            }
            else{
                putc('\n');
                // putc(buffer[i]);
            }
        }

        else if(buffer[i] == '\0'){
            continue;
        }

        else{
            putc(buffer[i]);
        }
    }

    // Updates cursor information / location
    upd_term_curs(pit_term, changePos(0, 0, 0, 0), changePos(0, 1, 0, 0));
    if(pit_term == active_term){
        update_cursor(changePos(0, 0, 0, 0), changePos(0, 1, 0, 0));
    }
    sti();

    return nbytes;
}

