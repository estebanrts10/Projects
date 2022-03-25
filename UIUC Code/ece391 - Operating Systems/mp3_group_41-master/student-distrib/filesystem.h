/* filesystem.h - Defines used for file system implementation */
#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include "types.h"

// Definitions used for file system implementation /
#define NUMBER_FILES    64
#define NUMBER_BLOCKS   4096
#define BLOCK_SIZE      4096
#define DENTRY_RESERVED 24
#define file_name_limit 32
#define STR_MAX         32
#define MAX_BLOCKS      1023
#define MAX_DIRECS      63
#define BOOT_RES        52        

extern uint32_t filesystem_address;

// Inode struct
typedef struct inode_t{
    uint32_t length;
    uint32_t block_nums[MAX_BLOCKS]; //number of blocks nimus bootblock
} inode_t;

// Directory entry struct
typedef struct dir_entry_t{
    int8_t   file_name[file_name_limit];
    uint32_t file_type;
    uint32_t inode;
    uint8_t  reserved[DENTRY_RESERVED]; // 24
} dir_entry_t;

// Boot block struct
typedef struct boot_block_t{ // how many for the first 3
    uint32_t           dir_entries;
    uint32_t           inodes;
    uint32_t           data_blocks;
    uint8_t            reserved[BOOT_RES]; //reserved space within bootblock
    dir_entry_t        direcs[MAX_DIRECS]; //number of dentries in bootblock
} boot_block_t;

// align -> __attribute__((aligned (NUMBER_FILES)))

// Function to allow us to open a block /
extern int block_open();

// Function to allow us to close a block /
extern int block_close();

// Function to allow us to read a block /
extern int block_read(int32_t fd, void* buf, int32_t nbytes);

// Function to allow us to write within a block /
extern int block_write(int32_t fd, void* buf, int32_t nbytes);

// Function to allow us to open a file /
extern int file_open();

// Function to allow us to close a file /
extern int file_close();

// find length of file in bytes
extern int find_file_length(void* fileToFind);

/* Function to allow us to read a file */
extern int file_read(int32_t fd, void* buf, int32_t nbytes);

/* Function to allow us to write within a file */
extern int file_write(int32_t fd, void* buf, int32_t nbytes);

/* helper functions */
extern int32_t read_dentry_by_name(const uint8_t* fname, dir_entry_t* dentry);

extern int32_t read_dentry_by_index(uint32_t index, dir_entry_t* dentry);

extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


#endif
