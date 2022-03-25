/* keyboard.c - Functions to initialize the keyboard */

#include "keyboard.h"
#include "paging.h"
#include "system_calls.h"
#include "scheduler.h"
#include "lib.h"
#include "systemcalls.h"

char read_buff[MAX_TERMS][READ_BUFF_SIZE];
unsigned int read_buff_end[MAX_TERMS] = {0, 0, 0};
volatile unsigned int enter_pressed[MAX_TERMS] = {0, 0, 0};

void shift_handler(uint8_t key);
void write_to_term(char charToWrite);
void prep_ret();

// flags
// int shift_on[MAX_TERMS] = {0, 0, 0};
int shift_on = 0;
int caps_on  = 0;
int alt_on   = 0;
int ctrl_on  = 0;
int in_prog  = 0;

// Array for character values to print to kernel
unsigned char set[CHAR_ARR_SIZE] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' ', '\0'};


/* keyboard_init()
 *
 * Initializes keyboard device
 * Inputs: None
 * Outputs: None
 * Side Effects: Enables IRQ 1 and initializes read buff
 * Coverage: Initializing keyboard
 * Files: kernel.c
 */
void keyboard_init() {
  in_prog = 0;
  clear_buff();
  enable_irq(KEYBOARD_IRQ);
  return;
}

/* clear_buff()
 * 
 * Inputs: none
 * Outputs: none
 * Function: clears buffer
 */
void clear_buff() {
  int i;
  for(i = 0; i < READ_BUFF_SIZE; i++){
    read_buff[active_term][i] = '\0';
  }
  return;
}

/* write_to_term()
 * 
 * Inputs: char charToWrite
 * Outputs: none
 * Function: writes a character to the terminal
 */
void write_to_term(char charToWrite){
  char writeBuffer[W_BUFF_SIZE];
  int x, y, curr_paging;
  from_keyboard = 1;
  writeBuffer[0] = charToWrite;                 // set buff to write and modify read buffer
  read_buff[active_term][read_buff_end[active_term]] = writeBuffer[0];
  read_buff_end[active_term]++;

  curr_paging = curr_vmem;        // save the current lib.c vmem pointer
  switch_to_screen();             // switch lib.c vmem pointer to screen 

  x = changePos(0, 0, 0, 0);
  y = changePos(0, 1, 0, 0);

  if(x == X_LIMIT){ // Check if X & Y coordinates are at edge of screen
    if(y == Y_LIMIT){   // Check to see if bottom most corner
      shift_vid_mem();  // moves video memory up one line
    }
    else{
      putc('\n');
    }
  }
  putc(charToWrite);

  // Update cursor information / location
  upd_term_curs(active_term, changePos(0, 0, 0, 0), changePos(0, 1, 0, 0));
  // Restore video memory to currently executing page
  restore_pit_buff(curr_paging);

  return;
}

/* prep_ret()
 * 
 * Inputs: none
 * Outputs: none
 * Function: prep return for keyboard handler
 */
void prep_ret(){
  from_keyboard = 0;
  in_prog = 0;
  send_eoi(KEYBOARD_IRQ);                       // send eoi and enable interupts
  sti();
  return;
}


/* keyboard_handler()
 *
 * Keyboard handler function for IDT
 * Inputs: None
 * Outputs: None
 * Side Effects: Outputs pressed key onto screen
 * Coverage: Load IDT
 * Files: idt_init.c
 */
void keyboard_handler() {
  if(in_prog == 1){
    send_eoi(KEYBOARD_IRQ);
    return;
  } 
  else{
    in_prog = 1;
  } 
  cli();
  int x, y, i, curr_paging;

  changePos(1, 0, termArr[active_term].curs_x, termArr[active_term].curs_y);
  from_keyboard = 1;
  x = changePos(0,0,0,0);                       // read x pos
  y = changePos(0,1,0,0);                       // read y pos

  if (inb(KEYBOARD_STAT) & STAT_CHECK) {        // Check status port before reading data
    uint8_t key = inb(KEYBOARD_DATA);           // Grab data from keyboard address
    if(key == K_ENTER){  
      curr_paging = curr_vmem;                  // save the current lib.c vmem pointer
      switch_to_screen();                       // switch lib.c vmem pointer to screen
      if(y == Y_LIMIT){
        shift_vid_mem();
      }
      else{
        putc('\n'); 
      }
      enter_pressed[active_term] = 1;                        // set enter flag and modify read buffer
      read_buff[active_term][read_buff_end[active_term]] = '\n';
      read_buff_end[active_term]++;
      upd_term_curs(active_term, changePos(0, 0, 0, 0), changePos(0, 1, 0, 0));
      restore_pit_buff(curr_paging);            // Restore video memory to currently executing page
      prep_ret();
      return;
    }
    if(key == K_CTRL_PRESS){
      ctrl_on = 1;                              // set ctrl flag
      prep_ret();
      return;
    }
    if(key == K_CTRL_RELEASE){
      ctrl_on = 0;                              // unset ctrl flag
      prep_ret();
      return;
    }
    if(key == K_ALT_PRESS){
      alt_on = 1;                              // set alt flag
      prep_ret();
      return;
    }
    if(key == K_ALT_RELEASE){
      alt_on = 0;                              // unset alt flag
      prep_ret();
      return;
    }
    if(alt_on){ // For multi terminals
      if(key == F_1){
        switch_terms(TERM1_ID);  // switch to term 1
        prep_ret();
        return;
      }
      if(key == F_2){
        switch_terms(TERM2_ID);  // switch to term 2
        prep_ret();
        return;
      }
      if(key == F_3){
        switch_terms(TERM3_ID);  // switch to term 3
        prep_ret();
        return;
      }
      if(key == K_4){
        from_keyboard = 1;
        printf("\n| %d | %d | %d | %d | %d | %d |\n", process_arr[0], process_arr[1], process_arr[2], process_arr[3], process_arr[4],  process_arr[5]);
        prep_ret();
        return;
      }

    }
    if(ctrl_on){
      if((key == K_L) || (key == (K_L-SH_OFF_NORM))){   
        curr_paging = curr_vmem;                // save the current lib.c vmem pointer
        switch_to_screen();                     // switch lib.c vmem pointer to screen
        clear();                                // clear screen on ctrl+L
        for(i=0; i<READ_BUFF_SIZE; i++){
          read_buff[active_term][i] = '\0';
        }
        read_buff_end[active_term] = 0;
        upd_term_curs(active_term, changePos(0, 0, 0, 0), changePos(0, 1, 0, 0));
        restore_pit_buff(curr_paging);          // Restore video memory to currently executing page
        prep_ret();
        return;
      }
    }
    if(key == K_BACK){     
      if((x == 0) && (y == 0)){                 // boundry condition for top left
        prep_ret();
        return;
      }
      if(read_buff_end[active_term] > 0){                    // general case to set read buffer
        read_buff[active_term][read_buff_end[active_term]] = '\0';
        read_buff_end[active_term]--;
      } 
      else {
        read_buff[active_term][0] = '\0';                    // case to set read buffer if size is 0
        read_buff_end[active_term] = 0;
        prep_ret();
        return;
      }
      curr_paging = curr_vmem;                  // save the current lib.c vmem pointer
      switch_to_screen();                       // switch lib.c vmem pointer to screen
      remc();
      upd_term_curs(active_term, changePos(0, 0, 0, 0), changePos(0, 1, 0, 0));
      restore_pit_buff(curr_paging);            // Restore video memory to currently executing page
      prep_ret();
      return;
    }
    if(read_buff_end[active_term] >= MAX_INPUT){ 
      prep_ret();
      return;
    }
    if(key == K_SPACE){
      write_to_term(' ');
      prep_ret();
      return;
    }
    if(key == K_LSHF_PRESS || key == K_RSHF_PRESS){   
      shift_on = 1;
      prep_ret();
      return;
    }
    if(key == K_LSHF_RELEASE || key == K_RSHF_RELEASE){   
      shift_on = 0;
      prep_ret();
      return;
    }
    if(shift_on && key <= KEYBOARD_RANGE){
      shift_handler(key);
      prep_ret();
      return;
    }
    if((key == K_CAPSL) && (caps_on==1)){       // case for when a letter is pressed and caps lock is on
      caps_on = 0;
      prep_ret();
      return;
    }
    else if((key == K_CAPSL) && (caps_on==0)){  
      caps_on = 1;
      prep_ret();
      return;
    }
    if((caps_on==1) && ((key>=K_Z && key<=K_M) || (key>=K_A && key<=K_L) || (key>=K_Q && key<=K_P))){
      write_to_term(set[key] - SH_OFF_NORM);
      prep_ret();
      return;
    }
    if(key <= KEYBOARD_RANGE){
      write_to_term(set[key]);
    }
  }
  prep_ret();
  return;
}

/* shift_handler()
 * 
 * Inputs: uint8_t key
 * Outputs: none
 * Function: writes special char to terminal
 */
void shift_handler(uint8_t key){
  //case of capital LETTER
  if(caps_on){
    if( (key>=K_Z && key<=K_M) || (key>=K_A && key<=K_L) || (key>=K_Q && key<=K_P)){
      write_to_term(set[key]);
      return;
    }
  }
  if((key>=K_Z && key<=K_M) || (key>=K_A && key<=K_L) || (key>=K_Q && key<=K_P)){
    write_to_term(set[key] - SH_OFF_NORM);
    return;
  }
  //case for !,#,$,%
  else if(key == 2 || key == 4 || key == 5 || key == 6){
    write_to_term(set[key] - SH_OFF_SPEC);
    return;
  }
  //case for @
  else if(key == 3){
    write_to_term('@');
    return;
  }
  //case for ^
  else if(key == 7){
    write_to_term('^');
    return;
  }
  //case for &
  else if(key == 8){
    write_to_term('&');
    return;
  }
  //case for *
  else if(key == 9){
    write_to_term('*');
    return;
  }
  //case for (
  else if(key == 10){
    write_to_term('(');
    return;
  }
  //case for )
  else if(key == 11){
    write_to_term(')');
    return;
  }
  //case for _
  else if(key == 0x0C){
    write_to_term('_');
    return;
  }
  //case for +
  else if(key == 0x0D){
    write_to_term('+');
    return;
  }
  //case for ~
  else if(key == 0x29){
    write_to_term('~');
    return;
  }
  //case for < > ?
  else if(key == 0x33 || key == 0x34 || key == 0x35){
    write_to_term(set[key] + SH_OFF_SPEC);
    return;
  } 
  //case for :
  else if(key == 0x27){
    write_to_term(':');
    return;
  }
  //case for "
  else if(key == 0x28){
    write_to_term('"');
    return;
  }
  //case for { } |
  else if(key == 0x1A || key == 0x1B || key == 0x2B){
    write_to_term(set[key] + SH_OFF_NORM);
    return;
  }
  return;
}
