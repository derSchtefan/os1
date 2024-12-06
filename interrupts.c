#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Structure for an IDT entry (8 bytes)
typedef struct {
    uint16_t base_low;  // Lower 16 bits of the handler function address
    uint16_t sel;       // Kernel segment selector
    uint8_t  always0;   // Always set to zero
    uint8_t  flags;     // Flags (type, attributes)
    uint16_t base_high; // Upper 16 bits of the handler function address
} __attribute__((packed)) idt_entry_t;

// Structure for the IDTR
typedef struct {
    uint16_t limit; // Size of the IDT - 1
    uint32_t base;  // Address of the IDT
} __attribute__((packed)) idt_ptr_t;

// Declare the IDT and IDTR
#define IDT_ENTRIES 256
idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idtr;


typedef struct
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t interrupt, error;

    uint32_t eip, cs, eflags;
} interruptFrame;


volatile int last_int_no = -1;
volatile int int_call_count = 0;

char *exceptions[] = {"Division By Zero",
                      "Debug",
                      "Non Maskable Interrupt",
                      "Breakpoint",
                      "Overflow",
                      "Out of Bounds",
                      "Invalid Opcode",
                      "Device Not Available",

                      "Double Fault",
                      "Coprocessor Segment Overrun",
                      "Invalid TSS",
                      "Segment Not Present",
                      "Stack-Segment Fault",
                      "General Protection Fault",
                      "Page Fault",
                      "Unknown Interrupt",

                      "x87 Floating-Point Exception",
                      "Alignment Check",
                      "Machine Check",
                      "SIMD Floating-Point Exception",
                      "Virtualization Exception",
                      "Control Protection Exception",
                
                      "Unknown Interrupt",
                      "Unknown Interrupt",
                      "Unknown Interrupt",
                      "Unknown Interrupt",
                      "Unknown Interrupt",
                      "Unknown Interrupt",
                      
                      "Hypervisor Injection Exception",
                      "VMM Communication Exception",
                      "Security Exception",
                      "Unknown Interrupt"};
                      
#define MASTER_PIC_COMMAND  0x20
#define SLAVE_PIC_COMMAND   0xA0
#define MASTER_PIC_DATA     0x21
#define SLAVE_PIC_DATA      0xA1

#define PIC_INIT            0x11

#define MASTER_PIC_OFFSET   0x20
#define SLAVE_PIC_OFFSET    0x28

#define PIC_8086_MODE       0x01

#define DEFAULT_MASTER_MASK 0x00
#define DEFAULT_SLAVE_MASK  0x00


void outportb(uint16_t _port, uint8_t _data) {
    __asm__ __volatile__("outb %1, %0" : : "dN"(_port), "a"(_data));
}

uint8_t inportb(uint16_t _port) {
    uint8_t rv;
  __asm__ __volatile__("inb %1, %0" : "=a"(rv) : "dN"(_port));
  return rv;
}

void remap_pic() {
    outportb(MASTER_PIC_COMMAND, PIC_INIT);         // Initialize on IO base address for Master PIC
    outportb(SLAVE_PIC_COMMAND, PIC_INIT);          // Initialize on IO base address for Slave PIC
    outportb(MASTER_PIC_DATA, MASTER_PIC_OFFSET);   // Master vector offset (starting at 32 since 0-31 exceptions)
    outportb(SLAVE_PIC_DATA, SLAVE_PIC_OFFSET);     // Slave vector offset
    outportb(MASTER_PIC_DATA, 0x04);                // Tells Master there is Slave PIC at IRQ2
    outportb(SLAVE_PIC_DATA, 0x02);                 // Slave PIC cascade identity
    outportb(MASTER_PIC_DATA, PIC_8086_MODE);       // Additional Master PIC environment info
    outportb(SLAVE_PIC_DATA, PIC_8086_MODE);        // Additional Slave PIC environment info
    outportb(MASTER_PIC_DATA, DEFAULT_MASTER_MASK); // Mask for Master PIC
    outportb(SLAVE_PIC_DATA, DEFAULT_SLAVE_MASK);   // Mask for Slave PIC
}

typedef void (*registered_irq_handler)(interruptFrame *regs);

registered_irq_handler irq_routines[16];

void registerIRQhandler(uint8_t id, registered_irq_handler handler) {
  irq_routines[id] = handler;
}



void central_interrupt_handler(interruptFrame framex) {
    interruptFrame* frame = &framex;
    last_int_no = frame->interrupt;
    int_call_count++;

    if (frame->interrupt >= 32 && frame->interrupt <= 47) {
        if (irq_routines[frame->interrupt - 32]) {
            registered_irq_handler handle = irq_routines[frame->interrupt - 32];
            handle(frame);
        }

        if (frame->interrupt >= 40) {
            outportb(0xA0, 0x20);
        }
        outportb(0x20, 0x20);
    } else if (frame->interrupt <=31) {
        asm volatile ("cli");
        printf("EXCEP: %s, CODE: %x", exceptions[frame->interrupt], frame->error );
        while(1) asm volatile ("hlt");
    } else if(frame->interrupt >= 48 && frame->interrupt <= 255) {
        // generic software interrupts
    } else {
        asm volatile ("cli");
        printf("PANIC rcvd int %i", frame->interrupt);
        while(1) asm volatile ("hlt");
    }
}


void interrupt_80h_handler(interruptFrame* frame) {
    printf("INT 0x80 HIT at %x\n", frame->interrupt);
}


// Function to set an IDT entry
void idt_set_entry(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;       // Lower 16 bits of address
    idt[num].base_high = (base >> 16) & 0xFFFF; // Upper 16 bits of address
    idt[num].sel = sel;                      // Segment selector
    idt[num].always0 = 0;                    // Reserved, always 0
    idt[num].flags = flags;                  // Flags (P, DPL, Type)
}

void isr_jump_table_start();

// Initialize the IDT
void idt_init() {
    idtr.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idtr.base = (uint32_t)&idt;

    uint32_t isr0_loc = &isr_jump_table_start;
    // Clear the IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_entry(i, isr0_loc+i*16, 0x08, 0x8E);
    }
    // Load the IDT
    asm volatile ("lidt (%0)" : : "r"(&idtr));

    
    remap_pic();
}