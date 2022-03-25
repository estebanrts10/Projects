#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "pagingasm.h"
#include "terminal.h"
#include "RTC.h"
#include "filesystem.h"
#include "systemcalls.h"
#include "system_calls.h"
#include "scheduler.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");    // Number is interrupt value (DO NOT CHANGE)
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	return result;
}
/* Divide_0_test
 *
 * Checks to see if divide by 0 exception occurs correctly
 * Inputs: None
 * Outputs: value of c
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int divide_0_test() {
    TEST_HEADER;
    int a = 1;
    int b = 0;
    int c = a / b;
    return c;
}

/* Checkpoint 2 tests */
void terminal_test(){
	char buffer[129];
	int r;

	while (1){
		pit_term = active_term;
        r = terminal_read(0, buffer, 150);
		pit_term = active_term;
        terminal_write(0, buffer, r);
		printf("|r %u | \n", r);
    }
}

/* Checkpoint 2 tests */
void rtc_test(){
	char buffer[2];
	unsigned int i;
	uint32_t freq;
	buffer[0] = '1';

	rtc_open();
	i = 0;
	while (1 == 1){
		if(i == 5){
			freq = 64;
			rtc_write(0, &freq, 0);
		}
		if(i == 85){
			freq = 256;
			rtc_write(0, &freq, 0);
		}
		if(!rtc_read(0, 0, 0)){
			terminal_write(0, buffer, 2);
			i++;
		}
    }
}

void read_by_name_test(){
	int j;
	uint8_t name[32] = "frame0.txt";
	dir_entry_t test0, test1;

	for(j=0; j<32; j++){
        test0.file_name[j] = '\0';
		test1.file_name[j] = '\0';
		// if(j==0){
		// 	name[j] = '';
		// }
		// else{
		// 	name[j] = '\0';
		// }
    }

	read_dentry_by_name(name, &test0);
	read_dentry_by_index(0, &test1);

    printf(" | dentry 1 filled - %s | \n", test0.file_name);
	printf(" | dentry 2 filled - %s | \n", test0.file_name);

	return;
}

void read_block_test(){
	dir_entry_t test0;
	uint8_t name[32] = "frame1.txt";
	uint8_t intro[12] = "fRead: ";
	uint8_t reset[10];
	int i, l;

	active_term = 0;
	
	while(1){
		terminal_write(0, intro, 10);
		terminal_read(0, name, 32);

		for(i=0; i<32; i++){
			if(name[i] == '\n'){
				name[i] = '\0';
			}
		}
		read_dentry_by_name(name, &test0);
		l = find_file_length(name);
		uint8_t file_buffer[l];
		// frame1.txt - inode 47 - 147 b
		file_read(test0.inode, (void*)file_buffer, l);

		terminal_write(0, file_buffer, l);

		printf("from func: %u \n", l);
		terminal_read(0, reset, 10);
	}
	

	// int j, ret;
	// int8_t name[32];
	// char newL = '\n';

	// block_open();

	// while (ret != -1){
	// 	ret = block_read(0, (void*)name, 0);
	// 	terminal_write(name, ret);
	// 	terminal_write(&newL, 1);
	// 	for(j=0; j<32; j++){
	// 		name[j] = '\0';
	// 	}
	// }

	return;
}

void timer_test(){
	int a;
	while (1){
		a++;
		if(a == 10){
			a = 0;
		}
	}
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	clear();
	// launch your tests here
    // divide_0_test();
	// terminal_test();
	// rtc_test();
	// read_by_name_test();
	read_block_test();
	// timer_test();

}
