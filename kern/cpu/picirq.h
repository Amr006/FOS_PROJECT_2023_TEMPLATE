/* See COPYRIGHT for copyright information. */

#ifndef FOS_KERN_PICIRQ_H
#define FOS_KERN_PICIRQ_H
#ifndef FOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#define MAX_IRQS	16	// Number of IRQs

// I/O Addresses of the two 8259A programmable interrupt controllers
#define IO_PIC1		0x20	// Master (IRQs 0-7)
#define IO_PIC2		0xA0	// Slave (IRQs 8-15)

#define IRQ_SLAVE	2	// IRQ at which slave connects to master
#define IRQ_OFFSET	32	// IRQ 0 corresponds to int IRQ_OFFSET

// test
#ifndef __ASSEMBLER__

#include <inc/types.h>
#include <inc/x86.h>

extern uint16 irq_mask_8259A;
void pic_init(void);
void irq_setmask_8259A(uint16 mask);

#endif // !__ASSEMBLER__

#endif // !JOS_KERN_PICIRQ_H
