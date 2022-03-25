#define     ASM     1
#include    "system_calls.h"
#include    "x86_desc.h"

.globl   halt_helper, execute_helper, read_helper, write_helper, open_helper, close_helper
.globl   getargs_helper, vidmap_helper, set_handler_helper, sigreturn_helper, generic_linkage_helper
.globl   final_pops, flush_tlb, finish_halt, execute_user_iret, base_shell_halt

#define IF_ON       0x0200
#define EXCEP_RET   256
#define CALL_MIN    1
#define CALL_MAX    10
#define ARG1        8
#define ARG2        12
#define ARG3        16
#define ARG4        20
#define ARG5        24

/* generic_linkage_helper
 *
 * Assembly linkage function to call system call functions / helpers
 * Inputs: EAX, EDX, ECX, EBX
 * Outputs: EAX
 * Side Effects: Saves and restores registers
 * Coverage: System calls
 * Files: systemcalls.c
 */
generic_linkage_helper:
    # Check if system call number passed through is valid (1 - 10)
    cmpl    $CALL_MIN, %eax
    jl      invalid_call
    cmpl    $CALL_MAX, %eax
    jg      invalid_call

    cmpl    $0, %eax
    je      invalid_call
    pushl   %ebp
    pushl   %esi
    pushl   %edi
    pushl   %edx 
    pushl   %ecx
    pushl   %ebx 
    pushfl
    # Parameters
    pushl   %edx # arg 3
    pushl   %ecx # arg 2
    pushl   %ebx # arg 1

    call    *jump_table(, %eax, 4)

final_pops:
    popl    %ebx
    popl    %ecx
    popl    %edx
    popfl
    popl    %ebx 
    popl    %ecx 
    popl    %edx 
    popl    %edi
    popl    %esi
    popl    %ebp
    iret

/* invalid_call
 *
 * Label to when eax is invalid
 * Inputs: EAX
 * Outputs: -1
 * Side Effects: None
 * Coverage: System calls
 * Files: systemcalls.c
 */
invalid_call:
    xorl    %eax, %eax
    movl    $-1, %eax
    leave
    ret

/* flush_tlb
 *
 * Clears tlb
 * Inputs: EBP
 * Outputs: EAX
 * Side Effects: Moves CR3 into EAX
 * Coverage: System calls
 * Files: systemcalls.c
 */
flush_tlb:
    pushl   %ebp
    movl    %esp, %ebp

    movl    %cr3, %eax
    movl    %eax, %cr3

    leave
    ret

base_shell_halt:
    leave
    ret

/* finish_halt
 *
 * Halts program
 * Inputs: EBP
 * Outputs: 
 * Side Effects: 
 * Coverage: System calls
 * Files: systemcalls.c
 */
finish_halt:
    pushl   %ebp
    movl    %esp, %ebp

    movl    ARG5(%ebp), %ecx  # excpt flag
    cmpl    $1, %ecx          # flag == 1
    je      re_shell

    movl    ARG4(%ebp), %ecx  # excpt flag
    movl    ARG1(%ebp), %eax  # status
    movl    ARG2(%ebp), %esp  # p esp
    movl    ARG3(%ebp), %ebp  # p ebp

    cmpl    $1, %ecx        # flag == 1
    jne     skip
    movl    $EXCEP_RET, %eax
skip:
    movl    %cr3, %ecx
    movl    %ecx, %cr3
    popl    %ecx
    leave
    ret
re_shell:
    movl    %ebp, %esp
    popl    %ebp
    jmp     reboot_sh


/* execute_user_iret
 *
 * IRET for user programs
 * Inputs: EBP
 * Outputs: 
 * Side Effects: 
 * Coverage: System calls
 * Files: systemcalls.c
 */
execute_user_iret:
    pushl   %ebp
    movl    %esp, %ebx      # save without messing with ebp

    movl    ARG1(%ebx), %eax  # get ss val
    movw    %ax, %ds
    pushl   %eax

    movl    ARG2(%ebx), %eax  # get esp val
    pushl   %eax

    pushfl  
    popl    %eax
    orl     $IF_ON, %eax
    pushl   %eax
    
    movl    ARG3(%ebx), %eax  # get cs val
    pushl   %eax
    movl    ARG4(%ebx), %eax  # get eip val
    pushl   %eax
    iret


jump_table:
        .long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
.end
