/* filesystem.C - Functions to allow access to specific files within the system*/

#include "filesystem.h"
#include "lib.h"
#include "terminal.h"
#include "systemcalls.h"

uint32_t filesystem_address;
uint32_t tracker;

/* block_open()
 *
 * Opens a directory 
 * Inputs: None
 * Outputs: 0
 * Side Effects: None
 * Coverage: None
 */
int block_open(){
    tracker = 0;
    return 0;
}

/* block_close()
 *
 * Closes a directory 
 * Inputs: None
 * Outputs: 0
 * Side Effects: None
 * Coverage: None
 */
int block_close(){
    tracker = 0;
    return 0;
}

/* block_read()
 *
 * Reads a directory
 * Inputs: File descriptor, buffer, number of bytes
 * Outputs: 0
 * Side Effects: None
 * Coverage: None
 */
int block_read(int32_t fd, void* buf, int32_t nbytes){
    dir_entry_t temp_dentry;
    boot_block_t* boot;
    int32_t dir_len, len, i;
    boot = (boot_block_t*)filesystem_address;
    dir_len = boot->dir_entries;

    // validity check
    if(buf == 0){
        return -1;
    }
    for(i=0; i < STR_MAX; i++){
        temp_dentry.file_name[i] = '\0';
    }

    if(dir_len > tracker){
        // get dentry of curr file then get len
        read_dentry_by_index(tracker, &temp_dentry);
        len = strlen(temp_dentry.file_name);

        if(len >= STR_MAX){
            // at most only copy 32 bytes to buffer
            memcpy(buf, (void *)temp_dentry.file_name, STR_MAX);
            // clear temp buffer
            for(i=0; i < STR_MAX; i++){
                temp_dentry.file_name[i] = '\0';
            }
            // move to next file
            tracker++;
            return STR_MAX;
        }
        else{
            // copy bytes and clear temp buffer
            memcpy(buf, (void *)temp_dentry.file_name, len);
            for(i=0; i < STR_MAX; i++){
                temp_dentry.file_name[i] = '\0';
            }
            // move to next file
            tracker++;
            return len;
        }
    }
    else{
        // reset file tracker
        tracker = 0;
        return 0;
    }
}

/* block_write()
 *
 * Writes to a directory 
 * Inputs: File descriptor, buffer, number of bytes
 * Outputs: -1
 * Side Effects: None
 * Coverage: None
 */
int block_write(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* file_open()
 *
 * Opens a file 
 * Inputs: None
 * Outputs: 0
 * Side Effects: None
 * Coverage: None
 */
int file_open(){
    return 0;
}

/* file_close()
 *
 * Closes a file 
 * Inputs: None
 * Outputs: 0
 * Side Effects: None
 * Coverage: None
 */
int file_close(){
    return 0;
}

/* find_file_length
 *
 * Find length of file
 * Inputs: File
 * Outputs: File length in bytes
 * Side Effects: None
 * Coverage: None
 */
int find_file_length(void* fileToFind){
    int length;
    dir_entry_t temp_dentry;
    inode_t* inode_arr;
    inode_t inode;
    uint8_t* file_name = (uint8_t *)fileToFind;

    // get dentry
    read_dentry_by_name(file_name, &temp_dentry);
    inode_arr = (inode_t *)(filesystem_address + BLOCK_SIZE);

    // get inode and length
    inode = inode_arr[temp_dentry.inode];
    length = inode.length;

    return length;
}

/* file_read()
 *
 * Reads a file 
 * Inputs: File descriptor, buffer, number of bytes
 * Outputs: -1 or index of the file
 * Side Effects: None
 * Function: gives us value indicating what file was read
 */
int file_read(int32_t fd, void* buf, int32_t nbytes){
    int i, offset, length, to_write;
    boot_block_t* boot;
    boot = (boot_block_t*)filesystem_address;
    uint32_t inodes_num = boot->inodes;
    uint32_t* block_nums;
    inode_t* inode_arr;
    inode_t inode;

    // set up pointers
    void* curr_block;
    void* to;
    void* data_start = (void *)(filesystem_address + ((1 + inodes_num) * BLOCK_SIZE));
    inode_arr = (inode_t *)(filesystem_address + BLOCK_SIZE);

    // get inode
    inode = inode_arr[fd];
    length = inode.length;
    block_nums = inode.block_nums;
    offset = 0;

    // set max length
    if(length > nbytes){
        length = nbytes;
    }

    // iterate through available blocks
    for(i=0; i < MAX_BLOCKS; i++){
        // go until we reach file size
        if(offset == length){
            break;
        }

        // set pointers
        to = buf + offset;
        curr_block = (void *)(data_start + (BLOCK_SIZE * block_nums[i]));

        // do copy
        if((length - offset) >= BLOCK_SIZE){
            memcpy(to, curr_block, BLOCK_SIZE);
            offset = offset + BLOCK_SIZE;
        }
        else{
            to_write = length-offset;
            memcpy(to, curr_block, to_write);
            offset = length;
        }
    }

    // return bytes read
    return offset;
}

/* file_write()
 *
 * Writes to a file 
 * Inputs: File descriptor, buffer, number of bytes
 * Outputs: -1
 * Side Effects: None
 * Coverage: None
 */
int file_write(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* read_dentry_by_name()
 *
 * Locates a specific dentry based of the given name 
 * Inputs: fname, dentry
 * Outputs: -1 or dentry with desired properties
 * Side Effects: None
 * Coverage: file_read uses
 */
int32_t read_dentry_by_name(const uint8_t* fname, dir_entry_t* dentry){
    int i, found, loc, cmp, len, dname_len;
    dir_entry_t* direcs_arr;
    int8_t new_fname[STR_MAX];
    boot_block_t* boot;
    boot = (boot_block_t*)filesystem_address;
    direcs_arr = (dir_entry_t*)boot->direcs;

    // clear input dentry
    for(i = 0; i < STR_MAX; i++){
        dentry->file_name[i] = '\0';
    }

    // get file info
    len = strlen((int8_t*)fname);
    if(len >= STR_MAX){
        strncpy(new_fname, (int8_t*)fname, STR_MAX);
    }
    else{
        strncpy(new_fname, (int8_t*)fname, len);
    }

    found = 0; //flag thats lets us know if we found dentry or not 
    for(i = 0; i < boot->dir_entries; i++){
        // search for name among available direc entrys
        dname_len = strlen(direcs_arr[i].file_name);
        if(dname_len >= STR_MAX){
            cmp = strncmp(new_fname, direcs_arr[i].file_name, STR_MAX);
        }
        else{
            cmp = strncmp(new_fname, direcs_arr[i].file_name, dname_len);
        }
        if(cmp == 0){
            found = 1; //indicates we found dentry
            loc = i;
            break;
        }
    }

    // if found copy info to input dentry
    if(found){
        if(dname_len >= STR_MAX){
            strncpy(dentry->file_name, direcs_arr[loc].file_name, STR_MAX);
        }
        else{
            strncpy(dentry->file_name, direcs_arr[loc].file_name, dname_len);
        }
        dentry->file_type = direcs_arr[loc].file_type;
        dentry->inode = direcs_arr[loc].inode;
        for(i = 0; i < DENTRY_RESERVED; i++){
            dentry->reserved[i] = boot->direcs[loc].reserved[i];
        }
        
        return (int32_t)dentry;
    }
    else{
        return -1;
    }
}

/* read_dentry_by_index()
 *
 * Locates a specific dentry based of the given index 
 * Inputs: index, dentry
 * Outputs: -1 or dentry with desired properties
 * Side Effects: None
 * Coverage: file_read uses
 */
int32_t read_dentry_by_index(uint32_t index, dir_entry_t* dentry){
    int i, found, len;
    dir_entry_t* direcs_arr;
    boot_block_t* boot;
    boot = (boot_block_t*)filesystem_address;
    direcs_arr = (dir_entry_t*)boot->direcs;

    found = 0; //flag thats lets us know if we found dentry or not 

    // search for index
    for(i = 0; i < boot->dir_entries; i++){
        if(i == index){
            found = 1; //indicates we found dentry
            break;
        }
    }

    // if index is valid copy info, else ret -1
    if(found){
        len = strlen(direcs_arr[index].file_name);
        if(len >= STR_MAX){
            strncpy(dentry->file_name, direcs_arr[index].file_name, STR_MAX);
        }
        else{
            strncpy(dentry->file_name, direcs_arr[index].file_name, len);
        }
        dentry->file_type = direcs_arr[index].file_type;
        dentry->inode = direcs_arr[index].inode;
        for(i = 0; i < DENTRY_RESERVED; i++){
            dentry->reserved[i] = boot->direcs[index].reserved[i];
        }
        return (int32_t)dentry;
    }
    else{
        return -1;
    }
}

/* read_data()
 *
 * Reads data at specific inode value
 * Inputs: inode, offset, buf, length
 * Outputs: -1 or i, numbr pf bits at specific point we need to read
 * Side Effects: None
 * Coverage: file_read and block_read uses
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

    return 0;
}
