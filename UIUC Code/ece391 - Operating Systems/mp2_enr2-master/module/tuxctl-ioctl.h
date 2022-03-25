// All necessary declarations for the Tux Controller driver must be in this file

#ifndef TUXCTL_H
#define TUXCTL_H

#define TUX_SET_LED _IOR('E', 0x10, unsigned long)
#define TUX_READ_LED _IOW('E', 0x11, unsigned long*)
#define TUX_BUTTONS _IOW('E', 0x12, unsigned long*)
#define TUX_INIT _IO('E', 0x13)
#define TUX_LED_REQUEST _IO('E', 0x14)
#define TUX_LED_ACK _IO('E', 0x15)
#define SECOND_BYTE_MASK
#define BYTE_MASK     0xFF
#define LOW_MASK      0xFFFF
#define LED_MASK      0xF0000
#define DECIMAL_MASK  0xF000000
#define FOURTHBIT     0x8
#define THIRDBIT      0x4
#define SECONDBIT     0x2
#define FIRSTBIT      0x1
#define HEX0_MASK     0xF
#define HEX1_MASK     0xF0
#define HEX2_MASK     0xF00
#define HEX3_MASK     0xF000

//extern int tux_init(struct tty_struct* tty);
//extern int set_led(struct tty_struct* tty, unsigned long arg);
//int tux_buttons(struct tty_struct* tty, unsigned cmd);

#endif
