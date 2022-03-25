;
; The code given to you here implements the histogram calculation that
; we developed in class.  In programming lab, we will add code that
; prints a number in hexadecimal to the monitor.
;
; Your assignment for this program is to combine these two pieces of
; code to print the histogram to the monitor.
;
; If you finish your program,
;    ** commit a working version to your repository  **
;    ** (and make a note of the repository version)! **


	.ORIG	x3000		; starting address is x3000


;
; Count the occurrences of each letter (A to Z) in an ASCII string
; terminated by a NUL character.  Lower case and upper case should
; be counted together, and a count also kept of all non-alphabetic
; characters (not counting the terminal NUL).
;
; The string starts at x4000.
;
; The resulting histogram (which will NOT be initialized in advance)
; should be stored starting at x3F00, with the non-alphabetic count
; at x3F00, and the count for each letter in x3F01 (A) through x3F1A (Z).
;
; table of register use in this part of the code
;    R0 holds a pointer to the histogram (x3F00)
;    R1 holds a pointer to the current position in the string
;       and as the loop count during histogram initialization
;    R2 holds the current character being counted
;       and is also used to point to the histogram entry
;    R3 holds the additive inverse of ASCII '@' (xFFC0)
;    R4 holds the difference between ASCII '@' and 'Z' (xFFE6)
;    R5 holds the difference between ASCII '@' and '`' (xFFE0)
;    R6 is used as a temporary register
;

	LD R0,HIST_ADDR      	; point R0 to the start of the histogram

	; fill the histogram with zeroes
	AND R6,R6,#0		; put a zero into R6
	LD R1,NUM_BINS		; initialize loop count to 27
	ADD R2,R0,#0		; copy start of histogram into R2

	; loop to fill histogram starts here
HFLOOP	STR R6,R2,#0		; write a zero into histogram
	ADD R2,R2,#1		; point to next histogram entry
	ADD R1,R1,#-1		; decrement loop count
	BRp HFLOOP		; continue until loop count reaches zero

	; initialize R1, R3, R4, and R5 from memory
	LD R3,NEG_AT		; set R3 to additive inverse of ASCII '@'
	LD R4,AT_MIN_Z		; set R4 to difference between ASCII '@' and 'Z'
	LD R5,AT_MIN_BQ		; set R5 to difference between ASCII '@' and '`'
	LD R1,STR_START		; point R1 to start of string

	; the counting loop starts here
COUNTLOOP
	LDR R2,R1,#0		; read the next character from the string
	BRz PRINT_HIST		; found the end of the string

	ADD R2,R2,R3		; subtract '@' from the character
	BRp AT_LEAST_A		; branch if > '@', i.e., >= 'A'
NON_ALPHA
	LDR R6,R0,#0		; load the non-alpha count
	ADD R6,R6,#1		; add one to it
	STR R6,R0,#0		; store the new non-alpha count
	BRnzp GET_NEXT		; branch to end of conditional structure
AT_LEAST_A
	ADD R6,R2,R4		; compare with 'Z'
	BRp MORE_THAN_Z         ; branch if > 'Z'

; note that we no longer need the current character
; so we can reuse R2 for the pointer to the correct
; histogram entry for incrementing
ALPHA	ADD R2,R2,R0		; point to correct histogram entry
	LDR R6,R2,#0		; load the count
	ADD R6,R6,#1		; add one to it
	STR R6,R2,#0		; store the new count
	BRnzp GET_NEXT		; branch to end of conditional structure

; subtracting as below yields the original character minus '`'
MORE_THAN_Z
	ADD R2,R2,R5		; subtract '`' - '@' from the character
	BRnz NON_ALPHA		; if <= '`', i.e., < 'a', go increment non-alpha
	ADD R6,R2,R4		; compare with 'z'
	BRnz ALPHA		; if <= 'z', go increment alpha count
	BRnzp NON_ALPHA		; otherwise, go increment non-alpha

GET_NEXT
	ADD R1,R1,#1		; point to next character in string
	BRnzp COUNTLOOP		; go to start of counting loop

	;BEGINNING OF MP CODE
	; This program works by taking in a string of values from
	; This program works by taking in the values from the given addresses
	; and going through three main loops. The first loop works by taking in
	; four bits and turning them into a hexadecimal value. Then the second
	; loop takes in four Ascii values and prints them out to the screen.
	; The third loop works by going through the second loop 27 times,
	; printing out as many lines of 16 bit hexadecimal values. In order
	;	to print numbers, letters, and symbols, the loops have to take in
	;	an offset and add it to the original value, changing it from
	;	hexadecimal to Ascii.
 	; partner: ry6

	PRINT_HIST
	;R0 -> Printer
	;R1 -> Digit counter
	;R2 -> Bit counter
	;R3 -> Histogram values
	;R4 -> “Digit”
	;R5 -> “Jack of all trades” (temporary register)
	;R6 -> Loop counter
	;R7 -> Trap vectors


	AND R6, R6, #0         		;initializing loop counter
	START_POINT AND R5, R5, #0

	LD R5, NUM_BINES         	;load -27 into R5, set as loop counter
	ADD R5, R5, R6         		;add -27 into R5
	BRzp DONE

	LD R5, AT_OFFSET        	;load @-z offset into R5
	ADD R0, R5, R6        		;add the @-z offset to the counter in order to have the required-
	;letter/symbol

	OUT                				;print to monitor
	LD R0, SPACE        			;load spacebar into r0
	OUT

	LD R5, HIST_ADDR        	;load starting histogram address into R5
	ADD R5, R6, R5        		;add histogram address to loop counter
	LDR R3, R5, #0        		;loads value in R5 into R3

	AND R1, R1, #0            ;initialize digit counter
	ADD R1, R1, #-4    				;set R1 as -4

	RESTART AND R5, R5, #0
	ADD R1, R1, #0            ;checking the if the value in r1 is currently 0 to halt
	BRzp HIST_END

	AND R2, R2, #0            ;initialize bit counter
	ADD R2, R2, #-4      		  ;set R2 as -4
	AND R4, R4, #0            ;initialize “digit”
	AND R5, R5, #0            ;reinitialize r5 for another check

	CHECK1 ADD R2, R2, #0     ;check if r2 is zero to go to print
	BRzp PRINTER              ;add a label later to move to where we print things

	ADD R4, R4, R4            ;shifting “digit” left
	ADD R3, R3, #0     			  ;check msb of R3

	BRzp #1           	 			;if the msb is 0 we want to skip the next step in which we add one

	ADD R4, R4, #1
	ADD R3, R3, R3            ;shifting R3 left
	ADD R2, R2, #1        		;increment bit counter
	BRnzp CHECK1

	PRINTER AND R5, R5, #0    ;reinitialize r5 for another check
	ADD R5, R5, #-5        		;next two lines wish to add -9 in order to subtract from “digit”-
	ADD R5, R5, #-4        		;to check if the hex value will be a letter or a number
	ADD R5, R5, R4

	AND R0, R0, #0        		;reinitialize R0
	ADD R0, R4, R0        		;place “digit” into R0
	ADD R5, R5, #0
	BRp LETTER            		;if “digit” > 9 go to “LETTER” otherwise continue
	LD R5, NUMBER_OFFSET
	ADD R0, R0, R5
	BRnzp DISPLAY

	LETTER LD R5, LETTER_OFFSET 	;loads R5 with offset for letters
	ADD R0, R0, R5            ;place offset into R0

	DISPLAY OUT

	ADD R1, R1, #1
	BRnzp RESTART


	HIST_END ADD R6, R6, #1 	;increment
	LD R0, NEW_LINE
	OUT                				;print next loop onto next line
	BRnzp START_POINT    			;when done with this part go and restart main loop



	DONE HALT            			;done


	; the data needed by the program
	LETTER_OFFSET .FILL x0037   ;ascii offset for the letter represented
	NUMBER_OFFSET .FILL x0030   ;Ascii offset for the number represented
	SPACE .FILL x0020  	   			;ascii value for spacebar
	AT_OFFSET .FILL x0040    		;the offset needed to print 0-26 as @-z
	NEW_LINE .FILL x000A    		;ascii value for a new line
	NUM_BINES .FILL xFFE5    		;-27 to check when the process is over and to use as offset
	NUM_BINS    .FILL #27       ; 27 loop iterations
	NEG_AT        .FILL xFFC0   ; the additive inverse of ASCII '@'
	AT_MIN_Z    .FILL xFFE6     ; the difference between ASCII '@' and 'Z'
	AT_MIN_BQ    .FILL xFFE0    ; the difference between ASCII '@' and '`'
	HIST_ADDR    .FILL x3F00    ; histogram starting address
	STR_START    .FILL x4000    ; string starting address

	; for testing, you can use the lines below to include the string in this
	; program...
	; STR_START    .FILL STRING    ; string starting address
	; STRING        .STRINGZ "This is a test of the counting frequency code.  AbCd...WxYz."



	    ; the directive below tells the assembler that the program is done
	    ; (so do not write any code below it!)

	    .END
