/* RTC.c - Functions to initialize the RTC */

#include "RTC.h"
#include "i8259.h"
#include "lib.h"

volatile int intrp_flag;

/* RTC_init()
 * 
 * Function to initialize RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: Enables IRQ 8
 * Coverage: Load IDT
 * Files: kernel.c
 */
void RTC_init() {
    char prev;
    intrp_flag = 0;

    // Disable nmi interupts
    outb(inb(RTC_IOPORT) | TURN_OFF_NMI, RTC_IOPORT);

    // Setting registers
    outb(REG_A, RTC_IOPORT);
    outb(REG_B, CMOS_IOPORT);

    // Turning on IRQ 8
    outb(REG_B, RTC_IOPORT);        
    prev = inb(CMOS_IOPORT);
    outb(REG_B, RTC_IOPORT);
    outb(prev | BIT_6_ON, CMOS_IOPORT);

    // Enable nmi interrupts
    outb(inb(RTC_IOPORT) & TURN_ON_NMI, RTC_IOPORT);

    // enable irq line
    enable_irq(RTC_IRQ);

    return;
}

/* rtc_open()
 * 
 * Inputs: None
 * Outputs: 0
 * Function: Opens RTC
 */
int rtc_open(){
    uint8_t prev, to_write;

    outb(REG_A, RTC_IOPORT);        
    prev = inb(CMOS_IOPORT) & MASK_TOP;

    to_write = prev | RATE_2;

    outb(REG_A, RTC_IOPORT);
    outb(to_write, CMOS_IOPORT);

    return 0;
}

/* rtc_read()
 * 
 * Inputs: File descriptor, buffer, number of bytes
 * Outputs: 0
 * Function: Reads RTC
 */
int rtc_read(int32_t fd, void* buf, int32_t nbytes){
    // reset int flag
    intrp_flag = 0;
    
    // wait for int
    while(intrp_flag == 0) {}

    // reset and return
    intrp_flag = 0;
    return 0;
}

/* rtc_write()
 * 
 * Inputs: File descriptor, buffer, number of bytes
 * Outputs: 0
 * Function: Writes to RTC
 */
int rtc_write(int32_t fd, void* buf, int32_t nbytes){
    uint32_t to_set;
    uint32_t * bufConv;
    uint8_t prev, to_write;
    cli();
    bufConv = (uint32_t *)buf;
    to_set = *bufConv;

    // validate freq
    if(!((to_set != 0) && !(to_set & (to_set - 1)))){
        return -1;
    }

    // get previous value of rtc reg
    outb(REG_A, RTC_IOPORT);        
    prev = inb(CMOS_IOPORT) & MASK_TOP;

    // modify value based on input
    switch (to_set){
        case 2:
            to_write = prev | RATE_2;
            break;
        case 4:
            to_write = prev | RATE_4;
            break;
        case 8:
            to_write = prev | RATE_8;
            break;
        case 16:
            to_write = prev | RATE_16;
            break;
        case 32:
            to_write = prev | RATE_32;
            break;
        case 64:
            to_write = prev | RATE_64;
            break;
        case 128:
            to_write = prev | RATE_128;
            break;
        case 256:
            to_write = prev | RATE_256;
            break;
        case 512:
            to_write = prev | RATE_512;
            break;
        case 1024:
            to_write = prev | RATE_1024;
            break;
        case 2048:
            to_write = prev | RATE_2048;
            break;
        case 4096:
            to_write = prev | RATE_4096;
            break;
        case 8192:
            to_write = prev | RATE_8192;
            break; 
        default:
            to_write = prev | RATE_2;
            break;
    }

    outb(REG_A, RTC_IOPORT);
    outb(to_write, CMOS_IOPORT);

    sti();

    return 0;
}

/* rtc_close()
 * 
 * Inputs: None
 * Outputs: 0
 * Function: Closes RTC
 */
int rtc_close(){
    return 0;
}

/* handle_rtc_int()
 * 
 * Handler function for RTC for use in IDT
 * Inputs: None
 * Outputs: None
 * Side Effects: Outputs flashing garbage values
 * Coverage: Load IDT
 * Files: idt_init.c
 */
void handle_rtc_int(){
    send_eoi(RTC_IRQ);
    
    intrp_flag = 1;

    // read regC and throw away
    outb(REG_C, RTC_IOPORT);
    inb(CMOS_IOPORT);    
    return;
}
