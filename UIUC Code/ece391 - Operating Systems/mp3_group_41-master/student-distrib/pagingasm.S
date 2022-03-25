#define     ASM     1
#include    "pagingasm.h"
#include    "x86_desc.h"

.globl   paging_helper, helper_long, helper_word, helper_byte, exception13

/* paging_helper()
 * 
 * Handler function for paging
 * Inputs: None
 * Outputs: None
 * Side Effects: Outputs flashing garbage values
 * Coverage: 
 * Files: 
 */
paging_helper:
    pushl   %ebp
    movl    %esp, %ebp

    movl    8(%esp), %eax
    andl    $0xfffff000, %eax
    movl    %eax, %cr3

    movl    %cr4, %eax
    orl     $0x00000010, %eax  # checks 5th bit to check for address extend?? 
    movl    %eax,%cr4

    movl    %cr0, %eax
    orl     $0x80000000, %eax
    movl    %eax, %cr0

    leave
    ret

helper_long:
    pushl   %ebp
    movl    %esp, %ebp

    movl    8(%esp), %eax
    leave
    ret

helper_word:
    pushl   %ebp
    movl    %esp, %ebp

    movw    8(%esp), %ax
    leave
    ret

helper_byte:
    pushl   %ebp
    movl    %esp, %ebp

    movb    8(%esp), %al
    leave
    ret


/* exception13()
 * 
 * test for excep 13
 * Inputs: None
 * Outputs: None
 * Side Effects: Outputs flashing garbage values
 * Coverage: Load IDT
 * Files: idt_init.c
 */
exception13:
    mov $0, %ax
    mov %ax, %ss
    ret 

