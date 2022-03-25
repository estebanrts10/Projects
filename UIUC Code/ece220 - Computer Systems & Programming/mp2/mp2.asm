
;Our program generates a calculator through the use of a stack. The main loop 
;takes in input from the keyboard and takes it to an evaluate loop, which in 
;turn goes to different subroutines based on if its a number, sign, or anything 
;else. It prints out invalid expression if the input doesn’t abide by the 
;reverse polish notation. By utilizing pop and push, we put numbers on stack, 
;and pop off 2 at a time when we encounter a sign from the keyboard. The 
;math for each sign is done in their respective subroutines, which loops 
;back to main loop afterwards to get a new input. If equal sign is detected, 
;the value still in stack is printed out through the use of the print 
;hexidecimal code we wrote in lab1. 

;parter: ry6 (Richard Yan)

.ORIG x3000

MAIN

AND R0, R0, #0
GETC				;gets input from keyboard and stores in R0
OUT
LD R1, SPACE_OFF
NOT R1, R1
ADD R1, R1, #1
ADD R1, R0, R1
BRz MAIN

;Test if character from keyboard is a number
AND R1, R1, #0

ADD R1, R0, #0 ; load char from keyboard into R1
LD R3, FIRST_CHAR_OFFSET ; put mem addr into R3
NOT R3, R3 ;
ADD R3, R3, #1 ; negate R3
ADD R1, R3, R1 ; find difference between offset and char value
BRn NON_NUMB ; if difference is negative, not a number -> print invalid
LD R3, SECOND_CHAR_OFFSET ; load R3 with offset from 9
AND R1, R1, #0 ;
NOT R3, R3 ;
ADD R3, R3, #1 ; negate R3
ADD R1, R0, #0 ; load char from keyboard into R1
ADD R1, R3, R1 ; find difference between offset and char value
BRp NON_NUMB ; if difference is positive, print invalid
LD R3, NUMBER_OFFSET
NOT R3, R3
ADD R3, R3, #1
ADD R0, R0, R3

JSR PUSH
JSR MAIN

NON_NUMB			;if the value is not a number the program runs through here
ST R0, CHAR_VAL		;and stores the value of the symbol for further use in evaluate
JSR EVALUATE

INVALID
LEA R0, END_STRING
PUTS
BRnzp FINISH
END_STRING .STRINGZ "INVALID EXPRESSION"

EQUAL
LD R1, STACK_TOP
LD R2, STACK_START			;when the program encounters an equal sign,
ADD R1, R1, #1				;it comes to this subroutine and either decides to print the value,
NOT R1, R1					;or print invalid expression
ADD R1, R1, #1
ADD R2, R1, R2
BRz PRINT_HEX
JSR INVALID



SEVEN_save .BLKW #1
FIRST_CHAR_OFFSET .FILL x0030
SECOND_CHAR_OFFSET .FILL x0039

CHAR_VAL .BLKW #1


FINISH
HALT


; PRINT_HEX
;   Description: prints out the value in R3 to the console
;   Inputs: R3 - value in register to be printed out
;   Outputs: Value printed to the console


PRINT_HEX
    JSR POP
    AND R3, R3, #0
    ADD R3, R3, R0
    AND R1, R1, #0            ;initialize digit counter
    ADD R1, R1, #-4                    ;set R1 as -4

    RESTART AND R5, R5, #0
    ADD R1, R1, #0            ;checking the if the value in r1 is currently 0 to halt
    BRzp FINISH

    AND R2, R2, #0            ;initialize bit counter
    ADD R2, R2, #-4                ;set R2 as -4
    AND R4, R4, #0            ;initialize “digit”
    AND R5, R5, #0            ;reinitialize r5 for another check

    CHECK1 ADD R2, R2, #0     ;check if r2 is zero to go to print
    BRzp PRINTER              ;add a label later to move to where we print things

    ADD R4, R4, R4            ;shifting “digit” left
    ADD R3, R3, #0                   ;check msb of R3

    BRzp #1                            ;if the msb is 0 we want to skip the next step in which we add one

    ADD R4, R4, #1
    ADD R3, R3, R3            ;shifting R3 left
    ADD R2, R2, #1                ;increment bit counter
    BRnzp CHECK1

    PRINTER AND R5, R5, #0    ;reinitialize r5 for another check
    ADD R5, R5, #-5                ;next two lines add -9 in order to subtract from“digit”-
    ADD R5, R5, #-4                ;to check if the hex value will be a letter or a number
    ADD R5, R5, R4

    AND R0, R0, #0                ;reinitialize R0
    ADD R0, R4, R0                ;place “digit” into R0
    ADD R5, R5, #0
    BRp LETTER                    ;if “digit” > 9 go to “LETTER” otherwise continue
    LD R5, NUMBER_OFFSET
    ADD R0, R0, R5
    BRnzp DISPLAY

    LETTER LD R5, LETTER_OFFSET     ;loads R5 with offset for letters
    ADD R0, R0, R5            ;place offset into R0

    DISPLAY OUT

    ADD R1, R1, #1
    BRnzp RESTART


; INSERT CODE HERE!



;ascii value for -,+,*,/ (respectively) --- x2D, x2B, x2A, x2F


MIN_OFF .FILL x002D
PLUS_OFF .FILL x002B
MULT_OFF .FILL x002A    ;labels for every offset
DIV_OFF .FILL x002F
EQUAL_OFF .FILL x003D
SPACE_OFF .FILL x0020


; EVALUATE
;   Description: handles input from console
;   Inputs: Console - input
;   Outputs: Console - evaluation of expression or error
;         R5 - evaluation of expression

EVALUATE

;Chooses what subroutine to go to

;everytime you get to an operand you pop the top 2
;R1 holds the value of the operand

LD R1, CHAR_VAL
LD R6, EQUAL_OFF
NOT R6, R6
ADD R6, R6, #1
ADD R6, R1, R6
BRz EQUAL

JSR POP ;                 ;these two consecutive pops are to get the two values that
AND R4, R4, #0            ;will be used
ADD R4, R4, R0            ;add the popped value into r3

JSR POP ;
ADD R5, R5, #0
BRnp INVALID      ;
AND R3, R3, #0            ;
ADD R3, R0, R3            ;add popped value into r4

LD R6, MIN_OFF            ;offset to check if it is the same sign that was input
NOT R6, R6                    
ADD R6, R6, #1            ;
ADD R6, R6, R1            ;
BRz  TO_MIN                        

LD R6, PLUS_OFF            ;offset to check if it is same sign as input
NOT R6, R6                    
ADD R6, R6, #1
ADD R6, R1, R6
BRz TO_PLUS                        

LD R6, MULT_OFF            ;offset to check if it is same sign as input
NOT R6, R6                    ;
ADD R6, R6, #1            ;
ADD R6, R6, R1            ;
BRz TO_MUL                            ;

LD R6, DIV_OFF            ;offset to check if it is same sign as input
NOT R6, R6
ADD R6, R6, #1
ADD R6, R6, R1
BRz TO_DIV
BRnzp INVALID                ;if its none of the other signs that means its invalid



TO_MIN						;makes using JSR to get to min and
JSR MIN						;back to main possible
JSR MAIN
							;same reasoning applies to the next three blocks of code
TO_PLUS						
JSR PLUS
JSR MAIN					

TO_MUL
JSR MUL
JSR MAIN

TO_DIV
JSR DIV
JSR MAIN


; PLUS
;   Description: adds two numbers (R0 = R3 + R4)
;   Inputs: R3 - addend
;        R4 - addend
;   Outputs: R0 - sum

PLUS

AND R0, R0, #0        ;initializes 0
ADD R0, R3, R4        ;add r3 and r4

ST R7, SEVEN_save    ;save r7 to be able to return back to_plus later
JSR PUSH                    ;push answer into stack
LD R7, SEVEN_save
RET


; INSERT CODE HERE!
; MIN
;   Description: subtracts two numbers (R0 = R3 - R4)
;   Inputs: R3 - minuend
;        R4 - subtrahend
;   Outputs: R0 - difference

MIN

NOT R4, R4;
ADD R4, R4, #1;        ;turns r4 negative
AND R0, R0, #0        ;re-initialize r0
ADD R0, R4, R3;        ;add them(subtracting)
ST R7, SEVEN_save
JSR PUSH                    ;pushing answer into the stack for next operation or print
LD R7, SEVEN_save
RET                             ;return back to_min which goes back to main loop


; INSERT CODE HERE!
;   Description: multiplies two numbers (R0 = R3 * R4)
;   Inputs: R3 - factor
;        R4 - factor
;   Outputs: R0 - product


MUL
;Inputs - R3, R4
AND R0, R0, #0        ;

ADD R4, R4, #0        ;setting up conditions for branch
BRz ZERO_MUL            ;if r4 is 0 go to zero_mul which outputs 0
ADD R3, R3, #0        ;same but for r3
BRz ZERO_MUL

ADD R3, R3, #0        ;setting conditions
BRn NEG_THREE            ; checking if r3 is negative
ADD R4, R4, #0        ;checking if r4 is negative
BRn NEG_FOUR


MULT_LOOP
ADD R0, R0, R3        ;multiplies r3 and r4
ADD R4, R4, #-1
BRp MULT_LOOP


ZERO                            ;label for branch from zero
ST R7, SEVEN_save    ; saving r7 for the nested JSR
JSR PUSH                    ;pushing value from the end of multiplication
LD R7, SEVEN_save ;loading back r7
RET                                ;returns to to_mult

ZERO_MUL                    ;if any operand is 0, set r0 to 0
AND R0, R0, #0
BRnzp ZERO

NEG_THREE                    ;if r3 is neg, first check if r4 is also neg
ADD R4, R4, #0
BRn BOTH_NEG            ;if both are negative then go to both_neg loop
NOT R3, R3                ;if just r3 is negative then turn it positive,
ADD R3, R3, #1        ;multiply, then turn the answer negative
MULT_LOOP3
ADD R0, R0, R3
ADD R4, R4, #-1
BRp MULT_LOOP3        ;multiplication for this loop
NOT R0, R0
ADD R0, R0, #1
BRnzp ZERO                ;go back to zero which is the ending of the mul loop

NEG_FOUR
NOT R4, R4
ADD R4, R4, #1        ;same as neg_three loop but with r4, and without
MULT_LOOP4                 ;checking if the other register is also neg
ADD R0, R0, R3
ADD R4, R4, #-1
BRp MULT_LOOP4
NOT R0, R0
ADD R0, R0, #1
BRnzp ZERO

BOTH_NEG
NOT R4, R4
ADD R4, R4, #1        ;if they are both negative, just simply turn them into
NOT R3, R3                ;positive numbers, and multiply
ADD R3, R3, #1
BRnzp MULT_LOOP        ;this can simply go back to the mult loop and multiply
                                     ;as pos numbers


; DIV
;   Description: divides two numbers (R0 = R3 / R4)
;   Inputs: R3 - numerator
;        R4 - denominator
;   Outputs: R0 - quotient
DIV

ADD R4, R4, #0        ;division by 0 is invalid
BRz INVALID
NOT R4, R4
ADD R4, R4, #1        ;turns r4 negative for subtraction(which turns into division)
AND R0, R0, #0
ADD R0, R0, #-1
DIV_LOOP                     ;loop to subtract numbers until r3 turns negative
ADD R0, R0, #1
ADD R3, R3, R4
BRzp DIV_LOOP

ST R7, SEVEN_save    ;saving r7 for when we return back to_div
JSR PUSH                    ;push answer into stack
LD R7, SEVEN_save
RET;


; PUSH
;   Description: Pushes a character unto the stack
;   Inputs: R0 - character to push unto the stack
;   Outputs: R5 - 0 (success) or 1 (failure/overflow)
;   Registers: R3 - stores STACK_END
;           R4 - stores STACK_TOP

PUSH


    ST R3, PUSH_SaveR3    ;save R3
    ST R4, PUSH_SaveR4    ;save R4
    AND R5, R5, #0        ;
    LD R3, STACK_END    ;
    LD R4, STACK_TOP    ;
    ADD R3, R3, #-1        ;
    NOT R3, R3        ;
    ADD R3, R3, #1        ;
    ADD R3, R3, R4        ;
    BRz OVERFLOW        ;stack is full
    STR R0, R4, #0        ;no overflow, store value in the stack
    ADD R4, R4, #-1        ;move top of the stack
    ST R4, STACK_TOP    ;store top of stack pointer
    BRnzp DONE_PUSH        ;

OVERFLOW

    ADD R5, R5, #1        ;

DONE_PUSH

    LD R3, PUSH_SaveR3    ;
    LD R4, PUSH_SaveR4    ;
    RET

PUSH_SaveR3    .BLKW #1    ;
PUSH_SaveR4    .BLKW #1    ;


; POP
;   Description: Pops a character off the stack
;   Inputs:
;   Outputs: R0 - character popped off the stack
;   Outputs: R0 - character popped off the stack
;         R5 - 0 (success) or 1 (failure/underflow)
;   Registers: R3 - stores STACK_END
;           R4 - stores STACK_TOP

POP
    ST R3, POP_SaveR3    ;save R3
    ST R4, POP_SaveR4    ;save R3
    AND R5, R5, #0        ;clear R5
    LD R3, STACK_START    ;
    LD R4, STACK_TOP    ;
    NOT R3, R3        ;
    ADD R3, R3, #1        ;
    ADD R3, R3, R4        ;
    BRz UNDERFLOW        ;
    ADD R4, R4, #1        ;
    LDR R0, R4, #0        ;
    ST R4, STACK_TOP    ;
    BRnzp DONE_POP        ;
UNDERFLOW
    ADD R5, R5, #1        ;
DONE_POP
    LD R3, POP_SaveR3    ;
    LD R4, POP_SaveR4    ;
    RET

LETTER_OFFSET .FILL x0037   ;ascii offset for the letter represented
NUMBER_OFFSET .FILL x0030   ;Ascii offset for the number represented
POP_SaveR3    .BLKW #1    ;
POP_SaveR4    .BLKW #1    ;
STACK_END    .FILL x3FF0    ;
STACK_START    .FILL x4000    ;
STACK_TOP    .FILL x4000    ;

.END
