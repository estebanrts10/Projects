/* RTC.h - Defines used to initialize the RTC */
#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "lib.h"
#include "x86_desc.h"

// Define constants to use for RTC initialization
#define RTC_IOPORT      0x70
#define CMOS_IOPORT     0x71
#define REG_A           0x0A
#define REG_B           0x0B
#define REG_C           0x0C
#define RTC_IRQ         8
#define BIT_6_ON        0x40
#define TURN_ON_NMI     0x7F
#define TURN_OFF_NMI    0x80
#define MASK_TOP        0xF0

#define RATE_2          0x0F
#define RATE_4          0x0E
#define RATE_8          0x0D
#define RATE_16         0x0C
#define RATE_32         0x0A
#define RATE_64         0x0A
#define RATE_128        0x09
#define RATE_256        0x08
#define RATE_512        0x07
#define RATE_1024       0x06
#define RATE_2048       0x05
#define RATE_4096       0x04
#define RATE_8192       0x03

/* Function to initalize RTC */
void RTC_init();
int rtc_open();
int rtc_write(int32_t fd, void* buf, int32_t nbytes);
int rtc_read();
int rtc_close();


#endif
