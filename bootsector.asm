BITS 16

BOOT_LOADER_STACK_START equ 0x7000
KERNEL_PROG_START   equ 0x000700
KERNEL_STACK_END    equ 0x100000
KERNEL_STACK_START  equ 0x1FFFFF
KERNEL_HEAP_START   equ 0x200000

[org 0x0500]
section BOOT start=0x0500
    cli
    ; setup segments
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov sp, BOOT_LOADER_STACK_START

    ; relocate to start of conventional memory
    mov di,0x0500
    mov si,0x7C00
    mov cx,512
    rep movsb

    jmp word 0:cs_reset
cs_reset:
    sti

    ; init 80x25 mode
    mov ah,0
    mov al,03h
    int 10h

    mov al, 'A'
    call printch


    mov ax, 0				; reset disk function
    int 13h					; call BIOS interrupt
    jc disk_reset_error

    mov ax, 0
    mov es, ax
	mov bx, KERNEL_PROG_START		; target
 
	mov ah, 2			; read sectors function
	mov al, byte [KERNEL_SECT_COUNT]	        ; number of sectors
	mov ch, 0			; cylinder number
	mov dh, 0			; head number
	mov cl, byte [KERNEL_SECT_START]		; starting sector number
    ; dl implicit from bios
	int 13h				; call BIOS interrupt
	jc disk_read_error


    mov al, 'B'
    call printch

cli
    lgdt [INITIAL_GDT_END_DESCRIPTOR]
    mov eax, cr0 
    or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov cr0, eax


   ; Reload CS register containing code selector:
   JMP   0x8:reload_CS ; 0x08 is a stand-in for your code segment
BITS 32

reload_CS:

BITS 32
    ; Reload data segment registers:
    MOV   EAX, 0x10 ; 0x10 is a stand-in for your data segment
    MOV   DS, EAX
    MOV   ES, EAX
    MOV   FS, EAX
    MOV   GS, EAX
    MOV   SS, EAX

    mov [DS:0x000b8004], byte '3'
    mov [DS:0x000b8005], byte 0x1F

    
; output boot message
    MOV EAX, BOOTING_KERNEL_MESSAGE
    MOV EDX, 80
    CALL STROUT_32BIT

; sti
    ; jump to next section
    MOV   ESP, KERNEL_STACK_START
    jmp CODE_SEL:KERNEL_PROG_START

BOOTING_KERNEL_MESSAGE: db '32 bit enabled. Interrupts disabled. Booting Kernel Loader', 0

STROUT_32BIT:
    shl EDX, 1
    add EDX, 0x000b8000
.top:
    mov BL, [EAX]
    test BL, BL
    je .exit
    mov byte [EDX], BL
    mov byte [EDX+1], 0x1F
    add edx, 2
    inc eax
    jmp .top

.exit:
    RET

BITS 16

disk_reset_error:
    mov al, 'D'
    jmp err

disk_read_error:
    mov al, 'R'
    jmp err

err:
    call printch
    jmp $


; char in AL
printch:
    mov ah, 0x0e
    int 0x10
    ret



; Define GDT entries
INITIAL_GDT_START:
NULL_SEL    equ $ - INITIAL_GDT_START
    db 0,0,0,0             ; Null segment
    db 0,0,0,0             ; Null segment
CODE_SEL    equ $ - INITIAL_GDT_START
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0
DATA_SEL    equ $ - INITIAL_GDT_START
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0
INITIAL_GDT_END:
INITIAL_GDT_END_DESCRIPTOR:
    dw INITIAL_GDT_END - INITIAL_GDT_START - 1
    dd INITIAL_GDT_START


times 508-($-$$) db 0
KERNEL_SECT_START db 2
;KERNEL_SECT_COUNT db 124  max
KERNEL_SECT_COUNT db 124
db 0x55, 0xaa
