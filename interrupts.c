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


typedef struct {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} interrupt_frame;

typedef struct {
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} interrupt_frame_with_error;


int last_int_no = 0;

// General exception handler
__attribute__((interrupt)) void exception_handler_with_error_code(interrupt_frame_with_error* frame)
{
    printf("Exception occurred at %x! Code: %x. Program halted\n", frame->eip, frame->error_code);
    while(1);
}

__attribute__((interrupt)) void exception_handler_no_error_code(interrupt_frame* frame) {
    printf("Exception occurred at %x! Program halted\n", frame->eip);
    while(1);
}


__attribute__((interrupt)) void interrupt_gen_handler(interrupt_frame* frame) {
    last_int_no++;
}

__attribute__((interrupt)) void interrupt_80h_handler(interrupt_frame* frame) {
    last_int_no++;
}


// Function to set an IDT entry
void idt_set_entry(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;       // Lower 16 bits of address
    idt[num].base_high = (base >> 16) & 0xFFFF; // Upper 16 bits of address
    idt[num].sel = sel;                      // Segment selector
    idt[num].always0 = 0;                    // Reserved, always 0
    idt[num].flags = flags;                  // Flags (P, DPL, Type)
}

// Initialize the IDT
void idt_init() {
    idtr.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idtr.base = (uint32_t)&idt;

    // Clear the IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_entry(i, 0, 0, 0);
    }

    idt_set_entry( 0, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Division Error
    idt_set_entry( 1, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Debug
    idt_set_entry( 2, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Non-maskable Interrupt
    idt_set_entry( 3, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Breakpoint
    idt_set_entry( 4, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Overflow
    idt_set_entry( 5, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Bound Range Exceeded
    idt_set_entry( 6, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Invalid Opcode
    idt_set_entry( 7, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Device Not Available
    idt_set_entry( 8, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Double Fault
    idt_set_entry( 9, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Coprocessor Segment Overrun
    idt_set_entry(10, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Invalid TSS
    idt_set_entry(11, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Segment Not Present
    idt_set_entry(12, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Stack-Segment Fault
    idt_set_entry(13, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // General Protection Fault
    idt_set_entry(14, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Page Fault
    idt_set_entry(15, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved
    idt_set_entry(16, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // x87 Floating-Point Exception
    idt_set_entry(17, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Alignment Check
    idt_set_entry(18, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Machine Check
    idt_set_entry(19, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // SIMD Floating-Point Exception
    idt_set_entry(20, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Virtualization Exception
    idt_set_entry(21, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Control Protection Exception
    idt_set_entry(22, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved
    idt_set_entry(23, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved
    idt_set_entry(24, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved
    idt_set_entry(25, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved
    idt_set_entry(26, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved
    idt_set_entry(27, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved
    idt_set_entry(28, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Hypervisor Injection Exception
    idt_set_entry(29, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // VMM Communication Exception
    idt_set_entry(30, (uint32_t)exception_handler_with_error_code, 0x08, 0x8E); // Security Exception
    idt_set_entry(31, (uint32_t)exception_handler_no_error_code, 0x08, 0x8E); // Reserved

    for(int i = 32; i <= 255; i++ ){
        idt_set_entry(i, (uint32_t)interrupt_gen_handler, 0x08, 0x8E); // Example: interrupts
    }

    idt_set_entry(0x80, (uint32_t)interrupt_80h_handler, 0x08, 0x8E); // Example: interrupts


    // Load the IDT
    asm volatile ("lidt (%0)" : : "r"(&idtr));
}