/* systemcalls.c - Functions for system calls */

#include "systemcalls.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystem.h"
#include "paging.h"
#include "terminal.h"
#include "RTC.h"
#include "system_calls.h"
#include "exception.h"
#include "scheduler.h"

// Any needed global variable
uint32_t excep_flag;
uint32_t num_process = 0;
int32_t process_arr[MAX_PROCS] = {0, 0, 0, 0, 0, 0};
proc_ctrl_t* task_proc_ctrl;
uint32_t openFiles = 0;
uint8_t file_buffer[FSIZE_MAX];
int init_base_shell = 0;


// CHECKPOINT 3

/* halt(uint8_t status)
 *
 * Halts a program
 * Inputs: status
 * Outputs: N/A
 * Side Effects: Jumps to execute after restoring parent PCB
 * Coverage: System calls
 * Files: systemcalls.c
 */
int32_t halt(uint8_t status){
    uint32_t status_32, i, esp, ebp, re_exec, proc_ind, parent_proc, save_excep_flag;

    // Getting esp & ebp from parent pcb
    esp = task_proc_ctrl->parent_stack;
    ebp = task_proc_ctrl->parent_base;

    // Checks to see if base shell
    if((ebp == 0) || (esp == 0)){ 
        re_exec = 1;    // if base shell, set re_execute flag
    }
    else{
        re_exec = 0;
        termArr[pit_term].curr_ebp = ebp;
        termArr[pit_term].curr_esp = esp;
    }

    // Only close fds 2 through 8 due to stdin & stdout
    for(i = FD_MIN; i < FD_MAX; i++){     
        close(i);
    }

    // Clear out process index value (0 indexed)
    proc_ind = task_proc_ctrl->proc_num;
    proc_ind--;
    process_arr[proc_ind] = 0;

    // Set current process to parent process
    parent_proc = task_proc_ctrl->parent_proc_num;
    termArr[pit_term].curr_proc_id = parent_proc;

    // Resetting number of processes for parent
    task_proc_ctrl->proc_num = 0;
    task_proc_ctrl->parent_proc_num = 0;

    // Clear video memory address
    if(task_proc_ctrl->vidmap_addr != 0){
        *task_proc_ctrl->vidmap_addr = 0;
        task_proc_ctrl->vidmap_addr = 0;
    }

    status_32 = (uint32_t)status;
    memset(task_proc_ctrl->argBuff, '\0', MAX_ARGS);    // Clears argument buffer
    
    // Restore Parent's process PCB as the one running, must change page directory 
    num_process--;
    task_proc_ctrl = (proc_ctrl_t *)(MB_8 - (KB_8 * parent_proc)); 
    init_prgm(parent_proc);

    // Load Parent's process kernel stack pointer into tss.esp0 
    tss.esp0 = (MB_8 - (KB_8 * parent_proc) + KB_8 - ESP_OFFSET);

    // Resetting exception flag
    save_excep_flag = excep_flag;
    excep_flag = 0;
    finish_halt(status_32, esp, ebp, save_excep_flag, re_exec);

    asm volatile (  ".globl  reboot_sh  \n\
                    reboot_sh:          ");
    init_base_shell = 1;
    execute((uint8_t *)("shell"));

    return 0;
}

/* execute(const uint* command)
 *
 * Executes a program
 * Inputs: command
 * Outputs: -1 if error, 256 if dies by exception, or value given by programs call to halt
 * Side Effects: Sets up programming paging and initializes PCB
 * Coverage: System calls
 * Files: systemcalls.c
 */
int32_t execute(const uint8_t* command){
    // Initialize variables
    dir_entry_t temp_dentry;
    int i = 0, length, valid, name_len, str_len, parent_proc;
    uint8_t fileNameToRun[STR_MAX];
    uint8_t argBuff_temp[MAX_ARGS];
    uint32_t argCount;
    uint32_t file_eip, ss_reg, cs_reg, file_esp;

    // If command invalid
    if (command == NULL) return -1;

    /* 1) Parse args */
    memset(fileNameToRun, '\0', STR_MAX);

    // Check for spaces, null characters, and new-lines
    while(command[i] != '\0' && command[i] != ' ' && command[i] != '\n'){
        fileNameToRun[i] = command[i];
        i++;
    }

    if(i >= STR_MAX){
        fileNameToRun[STR_MAX] = '\0';
    }
    else{
        fileNameToRun[i] = '\0';
    }

    // Clear argument buffer
    memset(argBuff_temp, '\0', MAX_ARGS);

    // Increment past space
    if(command[i] == ' '){
        i++;
    }

    argCount = 0;
    while(command[i] != '\0' && command[i] != '\n'){
        if(i == (MAX_ARGS - 1)){    // End of argument
            break;
        }
        else{   // Set argument buffer
            argBuff_temp[argCount] = command[i];
            argCount++;
            i++;
        }
    }
    
    argBuff_temp[i] = '\0';

    // Check if file is valid
    valid = read_dentry_by_name(fileNameToRun, &temp_dentry);
    if(valid == -1){
        return -1;
    }

    // Check if file names are the same
    name_len = strlen((int8_t *)temp_dentry.file_name);
    str_len  = strlen((int8_t *)fileNameToRun);
    if(str_len > STR_MAX){
        return -1;
    }
    if(name_len >= str_len){
        valid = strncmp((int8_t *)fileNameToRun, (int8_t *)temp_dentry.file_name, name_len);
    }
    else{
        valid = strncmp((int8_t *)fileNameToRun, (int8_t *)temp_dentry.file_name, str_len);
    }
    if(valid != 0){
        return -1;
    }

    length = find_file_length(fileNameToRun);
    
    /* 2) Executable Checks */ 
    file_read(temp_dentry.inode, file_buffer, length);

    // Check if file are executables
    if((file_buffer[0] != EXE_CHECK1) && (file_buffer[1] != EXE_CHECK2) && (file_buffer[2] != EXE_CHECK3) && (file_buffer[3] != EXE_CHECK4)){
        return -1;
    }

    /* 3) Find and create PCB */ 
    valid = 0;

    // Search for open process spot
    for(i = 0; i < MAX_PROCS; i++){
        if(process_arr[i] == 0){
            process_arr[i] = 1;
            valid = i;
            valid++;
            break;
        }
    }

    // If no open process spot available
    if(valid == 0){
        return -1;
    }

    num_process++;    // increment number of processes

    // If initializing base shell, set parent processes to 0
    if(init_base_shell){
        parent_proc = 0;
    }
    else{
        parent_proc = task_proc_ctrl->proc_num;
    }

    // Set PCB address & variables
    task_proc_ctrl = (proc_ctrl_t *)(MB_8 - (KB_8 * valid));
    task_proc_ctrl->proc_num = valid;
    task_proc_ctrl->parent_proc_num = parent_proc;

    // Scheduling; set terminal process to be active
    termArr[pit_term].curr_proc_id = task_proc_ctrl->proc_num;

    /* 4) Set up programming paging */ 
    init_prgm(task_proc_ctrl->proc_num);
    flush_tlb();

    /* 5) User-level Program Loader */ 
    memcpy((void *)UPRGM_ADDR, (void *)file_buffer, length);    // copies memory from file buffer to user page

    // Initialize PCB values
    task_proc_ctrl->fd_table[0].fops.open  = 0;                 // stdin
    task_proc_ctrl->fd_table[0].fops.close = 0;                 // stdin
    task_proc_ctrl->fd_table[0].fops.read  = &terminal_read;    // stdin
    task_proc_ctrl->fd_table[0].fops.write = 0;                 // stdin
    task_proc_ctrl->fd_table[0].flags = 1;

    task_proc_ctrl->fd_table[1].fops.open  = 0;                 // stdout
    task_proc_ctrl->fd_table[1].fops.close = 0;                 // stdout
    task_proc_ctrl->fd_table[1].fops.read  = 0;                 // stdout
    task_proc_ctrl->fd_table[1].fops.write = &terminal_write;   // stdout
    task_proc_ctrl->fd_table[1].flags = 1;

    // Setting PCB to PIT terminal & clearing vidmap address
    task_proc_ctrl->term_num = pit_term;
    task_proc_ctrl->vidmap_addr = 0;

    // Copies memory from local argument buffer to PCB argument buffer
    memcpy((void *)task_proc_ctrl->argBuff, (void *)argBuff_temp, MAX_ARGS);

    // Turn off the rest of the file descriptors
    for(i = FD_MIN; i < FD_MAX; i++){             
        task_proc_ctrl->fd_table[i].flags = 0;
    }

    // Check if a parent task or child task
    if(init_base_shell){
        task_proc_ctrl->parent_base = 0x00;    // set %EBP to 0 if first / only task
        task_proc_ctrl->parent_stack = 0x00;   // set %ESP to 0 if first / only task
        init_base_shell = 0;
    }
    else {  // Child task
        asm volatile (  "                   \n\
                        movl   %%esp, %0    \n\
                        movl   %%ebp, %1    \n\
                        "
                        :"=r"(task_proc_ctrl->parent_stack), "=r"(task_proc_ctrl->parent_base) // output
                    );
    }

    /* 6) Context Switch */ 
    tss.esp0 = (MB_8 - (KB_8 * task_proc_ctrl->proc_num) + KB_8 - ESP_OFFSET);

    // get program starting point from file buffer 24-27
    file_eip = (file_buffer[27] <<  BYTE3) + (file_buffer[26] <<  BYTE2) + (file_buffer[25] <<  BYTE1) + file_buffer[24];

    // Set needed registers for iret
    ss_reg = USER_DS;
    cs_reg = USER_CS;
    file_esp = ESP_INIT;
    excep_flag = 0;

    execute_user_iret(ss_reg, file_esp, cs_reg, file_eip);

    return 0;
}

/* read(int32_t fd, void* buf, int32_t nbytes)
 *
 * Reads a file
 * Inputs: file descriptor, buffer, number of bytes
 * Outputs: -1 if error, number of bytes read
 * Side Effects: N/A
 * Coverage: System calls
 * Files: systemcalls.c
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    uint32_t ret_val; 
    int32_t inode, f_len, b_to_copy, b_to_clear;
    void *begin;
    // Checks if valid file descriptor and buffer are inputted
    if(buf == NULL){          
        return -1;
    }

    // Checks if file descriptor is within bounds
    if(fd < 0){                             
        return -1;
    }
    if(fd > (FD_MAX-1) ){     // max index file descriptor
        return -1;
    }
    if(fd == 1){    // Don't allow read to open stdout
        return -1;
    }

    // Checks if file is in use 
    if(task_proc_ctrl->fd_table[fd].flags == 0){
        return -1;
    }

    // Checking file type & return number of bytes read
    if(task_proc_ctrl->fd_table[fd].file_type == FILE_F){
        f_len = find_file_length(task_proc_ctrl->fd_table[fd].name);
        inode = task_proc_ctrl->fd_table[fd].inode_num; 

        // If file position is at the end of file, end read
        if(task_proc_ctrl->fd_table[fd].file_pos >= f_len){
            task_proc_ctrl->fd_table[fd].file_pos = 0;
            memset(file_buffer, '\0', f_len);
            return 0; 
        }
        else{
            // Input inode for file read instead of file descriptor
            task_proc_ctrl->fd_table[fd].fops.read(inode, file_buffer, f_len);
            begin = (void *)(file_buffer + task_proc_ctrl->fd_table[fd].file_pos);

            // Read all bytes of buffer
            if((f_len - task_proc_ctrl->fd_table[fd].file_pos) >= nbytes){
                b_to_copy = nbytes;
                memcpy(buf, begin, b_to_copy);
            }
            // If on final read, clear unused bytes in input buffer
            else{
                b_to_copy = f_len - task_proc_ctrl->fd_table[fd].file_pos;
                b_to_clear = nbytes - b_to_copy;
                memcpy(buf, begin, b_to_copy);
                begin = (void *)(buf + b_to_copy);
                memset(begin, '\0', b_to_clear);
            }
            // Update file position
            task_proc_ctrl->fd_table[fd].file_pos = task_proc_ctrl->fd_table[fd].file_pos + b_to_copy;
            return b_to_copy;
        }
    }
    else{
        // Set interrupt flag for RTC reads
        sti();
        ret_val = task_proc_ctrl->fd_table[fd].fops.read(fd, buf, nbytes);
        return ret_val;
    }
}

/* write(int32_t fd, const void* buf, int32_t nbytes)
 *
 * Writes to a file
 * Inputs: file descriptor, buffer, number of bytes
 * Outputs: -1 if error, number of bytes written
 * Side Effects: N/A
 * Coverage: System calls
 * Files: systemcalls.c
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    uint32_t ret_val;
    int32_t inode;

    // Checks if valid file descriptor and buffer are inputted
    if(buf == NULL){ 
        return -1;
    }

    // Checks if file descriptor is within bounds
    if(fd < 0){
        return -1;
    }
    if(fd > (FD_MAX-1) ){ // If greater than max file descriptor index
        return -1;
    }
    if(fd == 0){    // Don't let write change stdin
        return -1;
    }

    // Checks if file is in use 
    if(task_proc_ctrl->fd_table[fd].flags == 0){ 
        return -1;
    }

    // Checking file type & return number of bytes written
    if(task_proc_ctrl->fd_table[fd].file_type == FILE_F){
        inode = task_proc_ctrl->fd_table[fd].inode_num; 
        ret_val = task_proc_ctrl->fd_table[fd].fops.write(inode, (void *)buf, nbytes);
        return ret_val;
    }
    else{
        ret_val = task_proc_ctrl->fd_table[fd].fops.write(fd, (void *)buf, nbytes);
        return ret_val;
    }
}

/* open(const uint8_t* filename)
 *
 * Opens a file
 * Inputs: Pointer to a file
 * Outputs: -1 if error, index of filename in descriptor
 * Side Effects: Files up FD table values
 * Coverage: System calls
 * Files: systemcalls.c
 */
int32_t open(const uint8_t* filename){
    dir_entry_t dentry;
    int i, flag, file_valid, dflen, flen;
    uint32_t flags_val;

    // Check if file exists
    if(filename == NULL){
        return -1;
    }

    // Check to see if any empty descriptors    
    flag = 0;
    for(i = FD_MIN; i < FD_MAX; i++){     // search between fd 2 and 8
        flags_val = task_proc_ctrl->fd_table[i].flags;
        if(flags_val == 0){
            flag = 1;
            break;
        }
    }

    // If no empty directories, return error
    if(flag == 0){
        return -1;
    }

    // Check if directory entry is valid
    file_valid = read_dentry_by_name(filename, &dentry);
    if(file_valid == -1){
        return -1;
    }

    // Check to see if filenames are the same
    dflen = strlen((int8_t *)dentry.file_name);
    flen  = strlen((int8_t *)filename);
    if(flen > STR_MAX){
        return -1;
    }
    if(dflen > STR_MAX){
        dflen = STR_MAX;
        flen = STR_MAX;
    }
    if(dflen >= flen){
        flag = strncmp((int8_t *)filename, (int8_t *)dentry.file_name, dflen);
    }
    else{
        flag = strncmp((int8_t *)filename, (int8_t *)dentry.file_name, flen);
    }
    if(flag != 0){
        return -1;
    }

    // Fill up FD table based on corresponding file type
    if(dentry.file_type == RTC_F){                  // RTC
        task_proc_ctrl->fd_table[i].flags = 1;
        task_proc_ctrl->fd_table[i].file_pos = 0;
        task_proc_ctrl->fd_table[i].file_type  = dentry.file_type;
        task_proc_ctrl->fd_table[i].fops.open  = &rtc_open;
        task_proc_ctrl->fd_table[i].fops.close = &rtc_close;
        task_proc_ctrl->fd_table[i].fops.read  = &rtc_read;
        task_proc_ctrl->fd_table[i].fops.write = &rtc_write;
        task_proc_ctrl->fd_table[i].fops.open();
        sti();
        openFiles++;
        return i;
    }
    if(dentry.file_type == DIR_F){                 // Dir
        task_proc_ctrl->fd_table[i].flags = 1;
        task_proc_ctrl->fd_table[i].file_pos = 0;
        task_proc_ctrl->fd_table[i].file_type  = dentry.file_type;
        task_proc_ctrl->fd_table[i].inode_num  = dentry.inode;
        task_proc_ctrl->fd_table[i].fops.open  = &block_open;
        task_proc_ctrl->fd_table[i].fops.close = &block_close;
        task_proc_ctrl->fd_table[i].fops.read  = &block_read;
        task_proc_ctrl->fd_table[i].fops.write = &block_write;
        task_proc_ctrl->fd_table[i].fops.open();
        openFiles++;
        return i;
    }
    if(dentry.file_type == FILE_F){                 // File
        task_proc_ctrl->fd_table[i].flags = 1;
        task_proc_ctrl->fd_table[i].file_pos = 0;
        task_proc_ctrl->fd_table[i].file_type  = dentry.file_type;
        task_proc_ctrl->fd_table[i].inode_num  = dentry.inode;
        task_proc_ctrl->fd_table[i].fops.open  = &file_open;
        task_proc_ctrl->fd_table[i].fops.close = &file_close;
        task_proc_ctrl->fd_table[i].fops.read  = &file_read;
        task_proc_ctrl->fd_table[i].fops.write = &file_write;
        strcpy((int8_t *)task_proc_ctrl->fd_table[i].name, (int8_t *)dentry.file_name);
        task_proc_ctrl->fd_table[i].fops.open();
        openFiles++;
        return i;
    }
    return -1;
}

/* close(int32_t fd)
 *
 * Closes a file
 * Inputs: File descriptor
 * Outputs: -1 if error, 0 if successful
 * Side Effects: Clears file descriptor
 * Coverage: System calls
 * Files: systemcalls.c
 */
int32_t close(int32_t fd){
    // Check to see if fd value is valid; don't close stdin and stdout    
    if(fd < FD_MIN){    
        return -1;
    }
    if(fd > (FD_MAX-1) ){
        return -1;
    }

    // Check to see if fd is in use
    if(task_proc_ctrl->fd_table[fd].flags == 0){
        return -1;
    }

    // Clear file descriptor name
    memset(task_proc_ctrl->fd_table[fd].name, '\0', STR_MAX);

    // Reset values of fd table
    task_proc_ctrl->fd_table[fd].flags = 0;
    task_proc_ctrl->fd_table[fd].file_pos = 0;
    task_proc_ctrl->fd_table[fd].inode_num = 0;
    openFiles--;
    return 0;
}

// CHECKPOINT 4

/* getargs (uint8_t* buf, int32_t nbytes)
 *
 * Gets number of arguments
 * Inputs: buffer, number of bytes
 * Outputs: 0 if successful, -1 if not
 * Side Effects: Puts number of arguments into buffer
 * Coverage: System calls
 * Files: systemcalls.c
 */
int32_t getargs (uint8_t* buf, int32_t nbytes) {
    void *from, *to;

    // Check if buffer & number of bytes is valid
    if(buf == NULL){
        return -1;
    }
    if(nbytes == 0){
        return -1;
    }
    if(task_proc_ctrl->argBuff[0] == 0){ // If no arguments
        return -1;
    }

    // Return arguments
    to = (void*)buf;
    from = (void*)task_proc_ctrl->argBuff;
    memset(to, '\0', nbytes);
    memcpy(to, from, nbytes);
    
    return 0;
}

/* vidmap()
 * 
 * Inputs: uint8_t** screen_start
 * Outputs: -1 if error, 0 if successful
 * Function: sets addr to vidmap page 
 */
int32_t vidmap (uint8_t** screen_start) {
    // Has to be within user page
    if(screen_start < (uint8_t **)VIDMAP_LOWER){
        return -1;
    }
    if(screen_start > (uint8_t **)VIDMAP_UPPER){
        return -1;
    }

    // Keep track of vidmap in process
    task_proc_ctrl->vidmap_addr = screen_start;

    // Initialize video memory page
    init_vid();
    flush_tlb();

    // Set address to re-mapped video memory pointer
    *screen_start = (uint8_t*)VIDMAP_UPPER;
    return 0;
}

/* Extra Credit */

int32_t set_handler (int32_t signum, void* handler_address) {
    return 0;
}

int32_t sigreturn (void) {
    return 0;
}
