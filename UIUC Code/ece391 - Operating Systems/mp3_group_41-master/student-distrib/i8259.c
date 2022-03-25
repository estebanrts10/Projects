/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/*
*   void i8259_init(void)
*   Inputs: none
*   Return Value: none
*	Function: initialize the necessary PIC slave and master ports
*/
void i8259_init(void) {
    // Masking interrupts on master & slave
    outb(MASK_ALL, MASTER_DATA);
    outb(MASK_ALL, SLAVE_DATA);

    // Initialize ports
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    outb(ICW2_MASTER, MASTER_DATA);
    outb(ICW2_SLAVE, SLAVE_DATA);

    outb(ICW3_MASTER, MASTER_DATA);
    outb(ICW3_SLAVE, SLAVE_DATA);

    outb(ICW4, MASTER_DATA);
    outb(ICW4, SLAVE_DATA);

    // Write desired mask to master & slave PIC
    outb(INIT_MASTER, MASTER_DATA);
    outb(INIT_SLAVE, SLAVE_DATA);
}

/*
*   void enable_irq(uint32_t irq_num)
*   Inputs: irq number
*   Return Value: none
*	Function: Enable (unmask) the specified IRQ
*/
void enable_irq(uint32_t irq_num) {
    uint32_t slave_irq;                              //initialize variables
    uint8_t master, slave, mask_to_use, final_mask;

    if(irq_num > INVALID_IRQ) {                      //check if the irq number falls between 0 and 15
        return;
    }

    master = inb(MASTER_DATA);                       //input data from starting port of master and the slave
    slave = inb(SLAVE_DATA);

    if(irq_num < MASTER_LIMIT) {                     //if the irq num is between 0-7
        mask_to_use = master;                        //then we enable the irq of the specified master port
        final_mask = mask_to_use & ~(1 << irq_num);  //by masking the irq number and the master information
        outb(final_mask, MASTER_DATA);
    }
    else {
        slave_irq = irq_num - MASTER_LIMIT;         //if the irq number is in the slave pic(8-15)
        mask_to_use = slave;                        //then we enable the irq of the specified slave port
        final_mask = mask_to_use & ~(1 << slave_irq);
        outb(final_mask, SLAVE_DATA);
    }
    return;
}

/*
*   void disable_irq(uint32_t irq_num)
*   Inputs: irq number
*   Return Value: none
*	Function: disable (mask) the specified IRQ
*/
void disable_irq(uint32_t irq_num) {
    uint32_t slave_irq;                             //initialize variables
    uint8_t master, slave, mask_to_use, final_mask;

    if (irq_num > INVALID_IRQ) {                    //check if the irq number falls between 0 and 15
        return;
    }

    master = inb(MASTER_DATA);                      //input data from starting port of master and the slave
    slave = inb(SLAVE_DATA);

    if (irq_num < MASTER_LIMIT) {                   //if the irq num is between 0-7
        mask_to_use = master;                       //then we disable the irq of the specified port
        final_mask = mask_to_use | (1 << irq_num);  //by masking the irq number and the specified master information
        outb(final_mask, MASTER_DATA);
    }
    else {
        slave_irq = irq_num - MASTER_LIMIT;         //if the irq number is in the slave pic(8-15)
        mask_to_use = slave;                        //then we disable the irq of the specified slave port
        final_mask = mask_to_use | (1 << slave_irq);
        outb(final_mask, SLAVE_DATA);
    }
    return;
}

/*
*   void send_eoi(uint32_t irq_num)
*   Inputs: irq number
*   Return Value: none
*	Function: Send end-of-interrupt signal for the specified IRQ
*/
void send_eoi(uint32_t irq_num) {
    uint32_t slave_irq_num;                     //initialize the slave irq number
    slave_irq_num = irq_num - MASTER_LIMIT;     //which is just the normal irq - 8

    if(irq_num > INVALID_IRQ) {                 //check if the irq number falls between 0 and 15
        return;
    }

    if (irq_num < MASTER_LIMIT) {               //if the irq num is between 0 and 7
        outb(EOI | irq_num, MASTER_8259_PORT);  //then send eoi to only the master
    }
    else {                                      //otherwise send to master and slave
        outb(EOI | slave_irq_num, SLAVE_8259_PORT);
        outb(EOI | SLAVE_PORT, MASTER_8259_PORT);          
    }
}
