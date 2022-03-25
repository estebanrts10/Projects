/* systemcalls.h - Defines used for system calls */
#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H

#include "types.h"
#include "filesystem.h"

/* Magic numbers that identifies files as an executable */
#define EXE_CHECK1      0x7F
#define EXE_CHECK2      0x45 
#define EXE_CHECK3      0x4C 
#define EXE_CHECK4      0x46 

#define MAX_ARGS        127
#define MB_8            0x800000
#define KB_8            8192
#define UPRGM_ADDR      0x08048000    
#define BYTE3           24
#define BYTE2           16 
#define BYTE1           8 
#define ESP_INIT        0x8400000
#define VIDMAP_UPPER    0x8400000
#define VIDMAP_LOWER    0x8000000

#define FD_MIN          2
#define FD_MAX          8

#define MAX_NAME        32
#define RTC_F           0
#define DIR_F           1
#define FILE_F          2
#define MAX_PROCS       6
#define FSIZE_MAX       36200

/* FOPS Table struct */
typedef struct fops_tbl {
    int32_t     (*open)();
    int32_t     (*close)();
    int32_t     (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t     (*write)(int32_t fd, void* buf, int32_t nbytes);
} fops_tbl_t;

/* File Descriptor struct */
typedef struct file_desc {
    fops_tbl_t  fops;
    uint32_t    inode_num;
    uint32_t    file_pos;
    uint32_t    flags;
    uint32_t    file_type;
    uint8_t     name[MAX_NAME];
} file_desc_t;

/* Process Control Block (PCB) struct */
typedef struct proc_ctrl {
    file_desc_t fd_table[FD_MAX];
    uint8_t     argBuff[MAX_ARGS];
    uint8_t**   vidmap_addr;
    uint32_t    term_num;
    uint32_t    parent_proc_num;
    uint32_t    proc_num;       // Number of particular process
    uint32_t    parent_stack;  // Parent registers
    uint32_t    parent_base;
} proc_ctrl_t;

extern uint32_t excep_flag;
extern uint32_t num_process;
extern int32_t process_arr[MAX_PROCS];
extern proc_ctrl_t* task_proc_ctrl;
extern int init_base_shell;

/* System Call Functions */
int32_t halt(uint8_t status);

int32_t execute(const uint8_t* command);

int32_t read(int32_t fd, void* buf, int32_t nbytes); 

int32_t write(int32_t fd, const void* buf, int32_t nbytes);

int32_t open(const uint8_t* filename);

int32_t close(int32_t fd);

int32_t getargs (uint8_t* buf, int32_t nbytes);

int32_t vidmap (uint8_t** screen_start);

int32_t set_handler (int32_t signum, void* handler_address);

int32_t sigreturn (void);

#endif
