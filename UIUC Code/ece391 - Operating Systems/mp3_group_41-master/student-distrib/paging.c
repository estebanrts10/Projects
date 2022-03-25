/* paging.c - Functions to initialize and enable the paging*/

#include "paging.h"
#include "pagingasm.h"
#include "system_calls.h"
#include "lib.h"
// page_dir direc;
page_direc_entry direc_entries[SIZE] __attribute__((aligned(PTE_SPACE)));
page table_entries[SIZE] __attribute__((aligned(PTE_SPACE)));
page vid_mem_tb[SIZE] __attribute__((aligned(PTE_SPACE)));

volatile int vid_mem_paging = 0;

/* paging_init()
 * 
 * initializes paging
 * Inputs: None
 * Outputs: None
 * Side Effects: sets up page tables and pages
 * Coverage: pages
 * Files: idt_init.c
 */
void paging_init(){
    int i, j;
    for(i=0; i<PDE_SPACE; i++){
        if(i == 0){
            for(j=0; j<PDE_SPACE; j++){
                if(j == VID_MEM_OFFSET){ 
                    table_entries[j].addr   = VIDEO_MEMORY >> ADDR_SHIFT;
                    table_entries[j].present= 1;
                    table_entries[j].r_w    = 1;
                }
                else if(j == BUFF1){
                    table_entries[j].addr   = BUFF1*KB_4 >> ADDR_SHIFT;
                    table_entries[j].present= 1;
                    table_entries[j].r_w    = 1;
                }
                else if(j == BUFF2){
                    table_entries[j].addr   = BUFF2*KB_4 >> ADDR_SHIFT;
                    table_entries[j].present= 1;
                    table_entries[j].r_w    = 1;
                }
                else if(j == BUFF3){
                    table_entries[j].addr   = BUFF3*KB_4 >> ADDR_SHIFT;
                    table_entries[j].present= 1;
                    table_entries[j].r_w    = 1;
                }
                else{
                    table_entries[j].addr   = j*KB_4 >> ADDR_SHIFT;
                    table_entries[j].present= 1;
                    table_entries[j].r_w    = 1;
                }
                table_entries[j].avail      = 0;
                table_entries[j].g_page     = 0; 
                table_entries[j].p_size     = 0; 
                table_entries[j].pt_written = 0;
                table_entries[j].accessed   = 0;
                table_entries[j].p_cd       = 0;
                table_entries[j].p_wt       = 0;
                table_entries[j].user_su    = 0;
            }
            // table_pointer = &table_entries;
            direc_entries[0].addr       = (unsigned int)table_entries >> ADDR_SHIFT; 
            direc_entries[0].avail      = 0;
            direc_entries[0].g_page     = 0; 
            direc_entries[0].p_size     = 0;
            direc_entries[0].pt_written = 0;
            direc_entries[0].accessed   = 0;
            direc_entries[0].p_cd       = 0;
            direc_entries[0].p_wt       = 0;
            direc_entries[0].user_su    = 0; 
            direc_entries[0].r_w        = 1; 
            direc_entries[0].present    = 1;
        }
        else if(i == 1){
            // fill kernal into direc
            direc_entries[1].addr       = KERNEL_ADDR >> ADDR_SHIFT; 
            direc_entries[1].avail      = 0;
            direc_entries[1].g_page     = 1; 
            direc_entries[1].p_size     = 1; // kernal is 4mb block
            direc_entries[1].pt_written = 0;
            direc_entries[1].accessed   = 0;
            direc_entries[1].p_cd       = 1;
            direc_entries[1].p_wt       = 0;
            direc_entries[1].user_su    = 0; 
            direc_entries[1].r_w        = 1; 
            direc_entries[1].present    = 1;
        }
        else{
            // should rest of direc be initialized to 0?
            direc_entries[i].present    = 0;
        }
    }

    ////////////////////////////////////////////////////

    paging_helper((unsigned int)direc_entries);

    return;
}

/* change_vmem()
 * 
 * Inputs: terminal to switch to
 * Outputs: None
 * Function: changes physical address of vmem 
 */
void change_vmem(int to_switch){
    switch (to_switch){
        case SCREEN:
            table_entries[VID_MEM_OFFSET].addr = VIDEO_MEMORY >> ADDR_SHIFT;
            break;
        case TERM1_BUFF:
            table_entries[VID_MEM_OFFSET].addr = BUFF1*KB_4 >> ADDR_SHIFT;
            break;
        case TERM2_BUFF:
            table_entries[VID_MEM_OFFSET].addr = BUFF2*KB_4 >> ADDR_SHIFT;
            break;
        case TERM3_BUFF:
            table_entries[VID_MEM_OFFSET].addr = BUFF3*KB_4 >> ADDR_SHIFT;
            break;
        default:
            table_entries[VID_MEM_OFFSET].addr = VIDEO_MEMORY >> ADDR_SHIFT;
            break;
    }
    vid_mem_paging = to_switch;
    flush_tlb();
    return;
}


/* init_prgm()
 * 
 * Inputs: process number
 * Outputs: None
 * Function: inits prgm 
 */
void init_prgm(uint32_t proc){
    direc_entries[USER_SPACE_PG].addr       = (USER_PRGM + (PRGM_OFFSET*proc)) >> ADDR_SHIFT; 
    direc_entries[USER_SPACE_PG].avail      = 0;
    direc_entries[USER_SPACE_PG].g_page     = 0; // also used for cacheing
    direc_entries[USER_SPACE_PG].p_size     = 1; // user mem is 4mb block
    direc_entries[USER_SPACE_PG].pt_written = 0;
    direc_entries[USER_SPACE_PG].accessed   = 0;
    direc_entries[USER_SPACE_PG].p_cd       = 1; // used for cacheing
    direc_entries[USER_SPACE_PG].p_wt       = 0;
    direc_entries[USER_SPACE_PG].user_su    = 1; 
    direc_entries[USER_SPACE_PG].r_w        = 1; 
    direc_entries[USER_SPACE_PG].present    = 1;
}


/* init_vid()
 * 
 * Inputs: None
 * Outputs: None
 * Function: inits vidmap page
 */
void init_vid(){
    int j;
    for(j=0; j<PDE_SPACE; j++){
        if(j == 0){ 
            vid_mem_tb[0].addr   = VIDEO_MEMORY >> ADDR_SHIFT;
            vid_mem_tb[0].present= 1;
            vid_mem_tb[0].r_w    = 1;
        }
        else{
            vid_mem_tb[j].addr   = j*KB_4 >> ADDR_SHIFT;;
            vid_mem_tb[j].present= 1;
            vid_mem_tb[j].r_w    = 1;
        }
        vid_mem_tb[j].avail      = 0;
        vid_mem_tb[j].g_page     = 0; 
        vid_mem_tb[j].p_size     = 0; 
        vid_mem_tb[j].pt_written = 0;
        vid_mem_tb[j].accessed   = 0;
        vid_mem_tb[j].p_cd       = 0;
        vid_mem_tb[j].p_wt       = 0;
        vid_mem_tb[j].user_su    = 1;
    }
    // table_pointer = &table_entries;
    direc_entries[USER_VID_PG].addr       = (unsigned int)vid_mem_tb >> ADDR_SHIFT; 
    direc_entries[USER_VID_PG].avail      = 0;
    direc_entries[USER_VID_PG].g_page     = 0; 
    direc_entries[USER_VID_PG].p_size     = 0;
    direc_entries[USER_VID_PG].pt_written = 0;
    direc_entries[USER_VID_PG].accessed   = 0;
    direc_entries[USER_VID_PG].p_cd       = 0;
    direc_entries[USER_VID_PG].p_wt       = 0;
    direc_entries[USER_VID_PG].user_su    = 1; 
    direc_entries[USER_VID_PG].r_w        = 1; 
    direc_entries[USER_VID_PG].present    = 1;
}

/* change_vid_map_page()
 * 
 * Inputs: terminal number to switch to
 * Outputs: None
 * Function: changes vidmap page
 */
void change_vid_map_page(int to_switch){
    switch (to_switch){
        case SCREEN:
            vid_mem_tb[0].addr = VIDEO_MEMORY >> ADDR_SHIFT;
            break;
        case TERM1_BUFF:
            vid_mem_tb[0].addr = BUFF1*KB_4 >> ADDR_SHIFT;
            break;
        case TERM2_BUFF:
            vid_mem_tb[0].addr = BUFF2*KB_4 >> ADDR_SHIFT;
            break;
        case TERM3_BUFF:
            vid_mem_tb[0].addr = BUFF3*KB_4 >> ADDR_SHIFT;
            break;
        default:
            vid_mem_tb[0].addr = VIDEO_MEMORY >> ADDR_SHIFT;
            break;
    }
    flush_tlb();
}

