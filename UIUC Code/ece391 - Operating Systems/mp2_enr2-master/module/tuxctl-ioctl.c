/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) \
	printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)
static unsigned char global_buffer[1];	//global buffer of size one used for the buttons pressed
static unsigned char led_buffer[6];			//global buffer necessary to restore the previous state of the leds
																				//6 is the size of the buffer(4 hexs, 1 opcode, 1 mask)
static int tux_init(struct tty_struct* tty);
static int set_led(struct tty_struct* tty, unsigned long arg);
volatile int led_flag;

/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in
 * tuxctl-ld.c. It calls this function, so all warnings there apply
 * here as well.
 */
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet)
{
    unsigned a, b, c;
		int right, left, down, up, cc, bb, aa, start;
    a = packet[0]; /* Avoid printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];


//on a reset, must reinitialize the leds, the bioc and then
//must place the global led_buffer(holding the led values) into the screen
		if(a == MTCP_RESET)
		{
			tux_init(tty);
			led_flag = 1;
			tuxctl_ldisc_put(tty,led_buffer,6);    //6 is the size of the led_buffer

		}
		if(a == MTCP_BIOC_EVENT){		//case that a button is pressed
			right = c & FOURTHBIT;
			down = c & THIRDBIT;
			left = c & SECONDBIT;
			up = c & FIRSTBIT;

/*		am masking the right, left, down, up bits found in
 *		the third byte of the packet to use in the global buffer
 *		am also masking c,b,a,start in for the same reasons
 *
*/		cc = b & FOURTHBIT;
			bb = b & THIRDBIT;
			aa = b & SECONDBIT;
			start = b & FIRSTBIT;

			right<<=4; 	//have to shift every direction bit 4 times left
			left<<=5; 	//except for left and down, must change 5 and 3
			down<<=3; 	//respectively because they are switching spots
			up<<=4;			//up also shifting 4 spots left
		//	global_buffer[0] = MTCP_BIOC_EVENT;
			global_buffer[0] = (right + left + down + up + cc + bb + aa + start)^(0xFF);

		}

		if(a == MTCP_ACK){
			if(led_flag==1){
				led_flag = 0;
			}
		}

    /*printk("packet : %x %x %x\n", a, b, c); */
}
/* tux_init(struct tty_struct* tty)
 * initializes the bioc to be on and initializes the led user.
 * must do this to be able to write to the leds and to get values from the buttons
 */
int tux_init(struct tty_struct* tty){
	char buffer[2];											// two is the size of the buffer
	buffer[0] = MTCP_LED_USR;
	buffer[1] = MTCP_BIOC_ON;
	tuxctl_ldisc_put(tty,buffer,2);			// two is the size of the buffer

	return 0;	//returns 0
}

/* set_led(struct tty_struct* tty, unsigned long arg)
 * sets leds in the spots that are needed in the tux controller,
 * translates the hex values inputted into the form needed to be displayed onto the
 * tux screen and then outputs them with the global buffer, saving them for later resets
 */

int set_led(struct tty_struct* tty, unsigned long arg){
	int hex0;
	int hex1;
	int hex2;
	int hex3;
	int dec0;
	int dec1;
	int dec2;
	int dec3;//initialize decimals
	int led0;
	int led1;		//initialize leds
	int led2;
	int led3;


	//hex table with led values for corresponding hex letters and numbers
	unsigned char hex_table[16] = { //16 is the amount of hex numbers and letters
		0xE7, // 0
		0x06, // 1
		0xCB, // 2
		0x8F, // 3
		0x2E, // 4
		0xAD, // 5
		0xED, // 6
		0x86, // 7
		0xEF, // 8
		0xAE, // 9
		0xEE, // A
		0x6D, // B
		0xE1, // C
		0x4F, // D
		0xE9, // E
		0xE8, // F
	};

	unsigned long low_sixteen = arg & LOW_MASK;
	unsigned long led_turnt = arg & LED_MASK;
	unsigned long decimal_turnt = arg & DECIMAL_MASK;

	//forloop that checks which leds should be turned on, (xxxx)
	int hex0_mask = HEX0_MASK & low_sixteen;
	int hex1_mask = HEX1_MASK & low_sixteen;
	int hex2_mask	= HEX2_MASK & low_sixteen;
	int hex3_mask = HEX3_MASK & low_sixteen;
	hex1_mask >>=4;		//must shift right 4 times to get hex1 into the first 4 bits
	hex2_mask >>=8;		//must shift right 8 times to get hex2 into the first 4 bits
	hex3_mask >>=12;  //must shift right 12 times to get hex3 into the first 4 bits

	decimal_turnt >>= 24;		//must shift decimal turnt to get decimal turned on values into the first 4 bits


	dec3 = decimal_turnt & FIRSTBIT;	//am masking decimals with their respective places to see if they are on or not
	dec2 = decimal_turnt & SECONDBIT;
	dec1 = decimal_turnt & THIRDBIT;
	dec0 = decimal_turnt & FOURTHBIT;

	dec0 <<= 1;				//once masked have to shift them to the 5th bit to have them fit into the hex values
	dec1 <<= 2;				//amount of shifting necessary for dec1-dec3 based on their positions
	dec2 <<= 3;				//dec0=1 spot,dec1=2,dec2=3,dec3=4
	dec3 <<= 4;


	led_turnt >>= 16;		//have to shift led_turnt based on its position in the 32 bit arg

	if(led_flag==1){		//check flag to see if we can write to the leds
		return -1;
	}

	led3 = led_turnt & FIRSTBIT;		//am masking leds with their respective places to see if they are on or not
	led2 = led_turnt & SECONDBIT;
	led1 = led_turnt & THIRDBIT;
	led0 = led_turnt & FOURTHBIT;

//just setting led0 to either 0 or FF to use it as a mask later
	if(led0 != 0)
		led0 = BYTE_MASK;
	else
		led0 = 0x00;
	if(led1 != 0)
		led1 = BYTE_MASK;
	else
		led1 = 0x00;
	if(led2 != 0)
		led2 = BYTE_MASK;
	else
		led2 = 0x00;
	if(led3 != 0)
		led3 = BYTE_MASK;
	else
		led3 = 0x00;

//getting led values from the hex table from the current hex values
	hex0 = hex_table[hex0_mask];
	hex1 = hex_table[hex1_mask];
	hex2 = hex_table[hex2_mask];
	hex3 = hex_table[hex3_mask];

//adding decimal where needed into the 8 hex bits(5th bit)
	hex0 |= dec3;
	hex1 |= dec2;
	hex2 |= dec1;
	hex3 |= dec0;

//masking led0-3 to see which should be turned on or off
	hex0 &= led0;
	hex1 &= led1;
	hex2 &= led2;
	hex3 &= led3;

/*	adding the led set opcode, the mask to know which leds to turn on, and then
 *	puts in each hex value into their corresponding spots in the led buffer
*/
	led_buffer[0] = MTCP_LED_SET;
	led_buffer[1] = HEX0_MASK;		//1 is the spot of the needed mask setting all to turned on
	led_buffer[2] = hex0;					//because hex values are already masked
	led_buffer[3] = hex1;					//2,3,4,5 are the corresponding spots of hex0-3
	led_buffer[4] = hex2;					//in the led set opcode.
	led_buffer[5] = hex3;
	led_flag = 1;
	tuxctl_ldisc_put(tty, led_buffer, 6); //6 is size of the led_buffer, putting it out into the screen
	return 0;
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/

/* set_led(struct tty_struct* tty, unsigned long arg)
 * ioctl function that calls the helper functions and does the necessary
 * operations to initialize, output button presses and set leds.
*/
int
tuxctl_ioctl (struct tty_struct* tty, struct file* file,
	      unsigned cmd, unsigned long arg)
{
    switch (cmd) {
	case TUX_INIT:
		return tux_init(tty); //call tux_init helper function, returns 0
	case TUX_BUTTONS:{
		int fail = copy_to_user((void*)arg,&global_buffer,1);
		//copies to user the button values and returns a value
		//checks if the copy to user failed or succeeded copying all the values into arg
		if(fail != 0){
			return -1;
		}
		return fail;
	}
	//set led case, just calls sed_led helper function and then returns 0
	case TUX_SET_LED:
		return set_led(tty, arg);
	case TUX_LED_ACK:
	case TUX_LED_REQUEST:
	case TUX_READ_LED:
	default:
	    return -EINVAL;
    }
}
