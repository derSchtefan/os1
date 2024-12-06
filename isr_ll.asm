[BITS 32]

global isr_jump_table_start
global isr_c_trampoline
section .text
align 16
isr_jump_table_start:

; Exceptions
%assign num 0
%rep 32
    align 16
    %if ((1 << num) & 0x9FDD82FF)  ; dodgy mask
        push dword 0
    %endif
    push dword num
    jmp isr_c_trampoline
    %assign num num + 1
%endrep

; IRQs
%rep 256 - 32
    align 16
    push dword 0
    push dword num
    jmp isr_c_trampoline
    %assign num num + 1
%endrep

extern central_interrupt_handler
isr_c_trampoline:
    push eax
    push ecx
    push edx
    push ebx
    push esp
    push ebp
    push esi
    push edi
    push ds
    push es
    push fs
    push gs

    ; Load kernel data segment
    ; this might be required for the future (v86 mode?)
    ;mov ax, 0x10
    ;mov ds, ax
    ;mov es, ax
    ;mov fs, ax
    ;mov gs, ax

    ; NOTE: if we would hard forece select the kernel code segment with 0x8:... we would get
    ; an additional element on the stack
    ; this might be required for the future (v86 mode?)
    call central_interrupt_handler

    pop gs
    pop fs
    pop es
    pop ds
    pop edi
    pop esi
    pop ebp
    pop esp
    pop ebx
    pop edx
    pop ecx
    pop eax

    add esp, 8

    iret
