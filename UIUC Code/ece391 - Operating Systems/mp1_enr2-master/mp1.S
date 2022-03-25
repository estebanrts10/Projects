
.data					# section declaration

        # Useful offset constants for accessing members of a
        # struct mp1_blink_struct structure
        LOCATION   = 0
        ON_CHAR    = 2
        OFF_CHAR   = 3
        ON_LENGTH  = 4
        OFF_LENGTH = 6
        COUNTDOWN  = 8
        STATUS     = 10
        NEXT       = 12

        STRUCT_SIZE = 16

        STACKnMOV       = 8
        SMALL_POPPER    = 4
        LARGE_POPPER    = 12
# Pointer to head of list (initialized to NULL)
mp1_list_head:
        .long   0

.text					# section declaration

# Export the function symbol names

.global mp1_rtc_tasklet
.global mp1_ioctl

# void mp1_poke(void);
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX
mp1_poke:

	movl    vmem_base_addr(,1),%edx
	movb    %cl,(%edx,%eax,1)
	ret

mp1_rtc_tasklet:
# 3rd
 # will go down the list head and exmaine each mp1_blink_struct
 # decrement the countdown
 # iff the countdown == 0 after decrement, examine status field
 # iff status = 1, then on_char char is on, 0 means char is off, this should put the opp chars


 pushl %ebp
 movl %esp, %ebp                # resetting base pointer

 pushl %ebx
 pushl %esi                     # callee saved registers
 pushl %edi
# push and pop the calee saved
xor %ebx, %ebx
 movl mp1_list_head, %ebx
 cmp $0, %ebx
 je endd
 list_loop:       # loops through the list in order to find the needed register

   decw COUNTDOWN(%ebx)
   cmpw $0, COUNTDOWN(%ebx)
   je startt
   # iff this countdown is 0 after decrement, check status field
   xor %ecx, %ecx
   movl NEXT(%ebx), %ecx        # looping through
   movl %ecx, %ebx              # putting ecx into ebx as a back up
   cmpl $0, %ebx                # checking to see if it is a valid struct
   je endd
   jmp list_loop
   startt:
    cmpw $1, STATUS(%ebx)       # checking if the dash is on or off
    je if_on
    cmpw $0, STATUS(%ebx)
    je if_off

   if_on:                       # turns off if it is on
   xor %ecx, %ecx
    xor %eax, %eax              # clearing registers
    movw LOCATION(%ebx), %ax
    addl %eax, %eax
    movb OFF_CHAR(%ebx), %cl    # turns off if it is on
      call mp1_poke             # setting changes
    movw OFF_LENGTH(%ebx), %dx
    movw %dx, COUNTDOWN(%ebx)   # placing length in countdown
    movw $0, STATUS(%ebx)       # changing status
    xor %ecx, %ecx
    movl NEXT(%ebx), %ecx
    movl %ecx, %ebx
    cmpl $0, %ebx
    je endd
    jmp list_loop

   if_off:                      # turns on if it is off
   xor %ecx, %ecx
    xor %eax, %eax              # clearing registers
    movw LOCATION(%ebx), %ax
    addl %eax, %eax
    movb ON_CHAR(%ebx), %cl
      call mp1_poke             # setting changes
    movw ON_LENGTH(%ebx), %dx
    movw %dx, COUNTDOWN(%ebx)   # placing length in countdown
    movw $1, STATUS(%ebx)       # changing status
    xor %ecx, %ecx
    movl NEXT(%ebx), %ecx
    movl %ecx, %ebx
    cmp $0, %ebx
    je endd
    jmp list_loop

 endd:
 popl %edi                      # callee saved registers popped back
 popl %esi
 popl %ebx
 leave
 ret


 mp1_ioctl:
 # 1st
   movl STACKnMOV(%esp), %edx # offset of 8, using cd and onlength to not use magic #
   cmp $3, %edx               # am using a 3 to cmp in order to see if i should return -1 or not
   jg return1
   cmp $0, %edx
   jl return1
   jmp *jump_table(,%edx,4)    # am not using POPPER because am not popping, using it
   # to displace 4 bytes every, because that is the size of each location ptr

   ret

 return1:
   movl $-1, %eax
   leave
   ret
 return0:
   movl $0, %eax
   leave
   ret


mp1_ioctl_add:
# 2nd
  # movl 2(%ebp), %ebx  #have to change offset, dont know what it is yet


  pushl %ebp
  movl %esp, %ebp           # resetting base pointer

  pushl %ebx
  pushl %esi                # callee saved registers
  pushl %edi

  xor %ebx, %ebx

  movl STACKnMOV(%ebp), %ebx
  cmpl $0, %ebx
  je return1

  pushl $STRUCT_SIZE
  # taking arg needed to call function, no need for ebx because of word

  call mp1_malloc           # dynamically allocating memory
  cmp $0, %eax
  je return1
  add $SMALL_POPPER, %esp
  movl %eax, %ecx


  pushl %ecx
  pushl $STRUCT_SIZE
  pushl %ebx                  # saving and setting up values for copy from user
  pushl %ecx

  call mp1_copy_from_user
  addl $LARGE_POPPER, %esp
  popl %ecx     # popping


  xor %edx, %edx
  movw ON_LENGTH(%ecx), %dx
  movw %dx, COUNTDOWN(%ecx)    # making the on length be the new countdown
  movw $1,STATUS(%ecx)

# use displacement function
  # ask a TA, changing values up there but might not be changing struct
  # countdown = on_length
  # status = 1
  movl mp1_list_head, %edx
  movl %edx, NEXT(%ecx)
  movl %ecx, mp1_list_head

  # make a call to mp1_poke

  # pushh the 2 arguments
  xor %eax, %eax
  movw LOCATION(%ecx), %ax
  addl %eax, %eax           # doubling the location because its 2byte addresses
  movb ON_CHAR(%ecx), %cl

  # cl is the ascii value of the letter to print
  call mp1_poke
  popl %edi
  popl %esi                 # callee saved registers
  popl %ebx
  movl $0, %eax
  leave
  ret

mp1_ioctl_remove:
  # mp1_ioctl_remove(unsigned long arg)
  # goes through mp1_list_head
  # finds if there is a element whos location matches argument
  # iff so remove it from linked  list and free its memory with mp1_free, return 0
  # ecx is the next reg and ebx is the previous
  pushl %ebp
  movl %esp, %ebp           # resetting base pointer
  pushl %ebx
  pushl %esi                # callee saved registers
  pushl %edi


  xor %ebx, %ebx            # resetting registers
  xor %ecx, %ecx
  xor %edx, %edx
  # movl mp1_list_head, %ebx
  # movw 8(%ebp), %cx

   movl mp1_list_head, %ebx
   movw STACKnMOV(%ebp), %dx
   cmpl $0, %ebx
   je remove_end              # check if its a valid ptr, otherwise end
   movl NEXT(%ebx), %ecx
   cmpw LOCATION(%ebx), %dx   # check if first node is the one we want to remove
   je remove_head_node
   remove_list_loop:
 	cmp LOCATION(%ecx), %dx     # otherwise loop through list
 	je remove_node

 	cmp $0,NEXT(%ecx)
 	je return1

 	movl %ecx, %ebx
 	movl NEXT(%ecx), %ecx

 	cmp $0, %ecx  	#
  jne remove_list_loop        # return to loop and continue, otherwise end
  movl $-1, %eax
  jmp remove_end

 	remove_node:
 	movl NEXT(%ecx), %eax
 	movl %eax, NEXT(%ebx)
 	pushl %ecx                  # removes any node that isnt the head
 	call mp1_free
  movl $0, %eax
 	jmp remove_end

 	remove_head_node:
 	movl NEXT(%ebx), %eax
 	movl %eax, mp1_list_head    # removes the head node if called
 	pushl %ebx
  movl $0, %eax
 	jmp remove_end


remove_end:
  popl %edi
  popl %esi                     # callee saved registers popped back
  popl %ebx
  leave
  ret

mp1_ioctl_find:
  pushl %ebp
  movl %esp, %ebp
  pushl %ebx
  pushl %esi                    # callee saved registers
  pushl %edi

  movl STACKnMOV(%ebp), %ebx
  cmp $0, %ebx
  je return1
  pushl $STRUCT_SIZE

  call mp1_malloc               # allocate needed memory

  cmp $0, %eax
  je return1
  addl $SMALL_POPPER, %esp
  movl %eax, %ecx


  pushl %ecx
  pushl $STRUCT_SIZE            # saving and storing values needed for copt from user
  pushl %ebx                    # or that are needed after and are caller saved
  pushl %ecx

  call mp1_copy_from_user
  cmpl $0, %eax
  je working
  movl $-1, %eax
  jmp endig

  working:
  addl $LARGE_POPPER, %esp      # continue if the struct was copied successfully
  popl %ecx

  xor %esi, %esi
  movl LOCATION(%ebx), %esi
  call mp1_search
  cmp $-1, %eax
  je endig

  pushl %eax              # saving ptr to free memory after calling copy to user
  pushl $STRUCT_SIZE
  pushl %ecx
  pushl %eax
  call mp1_copy_to_user   # now copying to the user, returning to user space
  addl $LARGE_POPPER, %esp
  call mp1_free           # free allocated memory
  addl $SMALL_POPPER, %esp

  movl $0, %eax

  endig:
  popl %edi
  popl %esi                         # callee saved registers popped back
  popl %ebx
  leave
	ret

mp1_ioctl_sync:
  pushl %ebp
  movl %esp, %ebp
  pushl %ebx
  pushl %esi                       # callee saved registers
  pushl %edi

  xor %ebx, %ebx
  xor %ecx, %ecx

  movl STACKnMOV(%ebp), %ecx
  movw %cx, %bx
  shrl $16, %ecx    # using 16 in order to shift the first half of the register -
  # values to the second half
# %cx is the 2nd number bx is the 1st num
  pushl %ebx
  call mp1_search                     # search to find location, check to see if its been found
  cmp $-1, %eax
  je end_sync
  movl %eax, %esi
  addl $SMALL_POPPER, %esp           # popping one ptr
  # esi is the bx and eax is cx
  pushl %ecx
  call mp1_search
  cmpl $-1, %eax
  je end_sync                       # doing the same as up there, but for other char
  addl $SMALL_POPPER, %esp
  movl %eax, %ecx

  xor %bx, %bx
  movw ON_LENGTH(%ecx), %bx
  movw %bx, ON_LENGTH(%esi)
  movw OFF_LENGTH(%ecx), %bx
  movw %bx, OFF_LENGTH(%esi)         # copying over all necessary values in order
  movw COUNTDOWN(%ecx), %bx          # to sync them together
  movw %bx, COUNTDOWN(%esi)
  movw STATUS(%ecx), %bx
  movw %bx, STATUS(%esi)

  xor %eax, %eax
  cmpw $1, STATUS(%esi)             # checking if its on or off to know which way to sync
  je on_sync

  movw LOCATION(%esi), %ax
  addl %eax, %eax
  movb OFF_CHAR(%esi), %cl
  movl $0, %eax
  jmp end_sync
                          # up for when the char is turned off, down for opposite
  on_sync:
  movw LOCATION(%esi), %ax
  addl %eax, %eax
  movb ON_CHAR(%esi), %cl
  movl $0, %eax

  end_sync:
  popl %edi
  popl %esi                         # callee saved registers popped back
  popl %ebx
  leave
	ret

mp1_search:
  pushl %ebp
  movl %esp, %ebp                   # resetting base and stack pointers
  pushl %ebx

  movl STACKnMOV(%ebp), %ebx        # make sure its actually mov6 not 8
  xor %eax, %eax
# ebx is the arg pushed in, generally the location
  movl mp1_list_head, %eax
# main loop for searching through the linked list to find what is necessary
  loopdidoo:
    cmpw LOCATION(%eax), %bx
    je finale
    movl %eax, %edx
    movl NEXT(%eax), %eax
    cmp $0, %eax
    jne loopdidoo
    movl $-1, %eax


  finale:
  popl %ebx
  leave
  ret

jump_table:       # jump table lul
  .long mp1_ioctl_add, mp1_ioctl_remove, mp1_ioctl_find, mp1_ioctl_sync
  ret

.end
