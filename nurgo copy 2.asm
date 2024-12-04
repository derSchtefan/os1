BITS 16

STACK_END equ 0x0500
STACK_START equ 0x7000
STACK_START_2 equ 0x7FFF
KERNEL_START equ 0x8000

[org 0x7c00]
section BOOT start=0x7C00
    cli
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov sp, STACK_START
    jmp 0:.cs_reset
.cs_reset:
    sti

    mov ah,0
    mov al,03h
    int 10h

    mov ax,0xb800
    mov fs,ax
    
    mov fs:0x0008, byte 'K'
    mov fs:0x0009, byte 0x1F
    
    mov ax,0
    mov fs,ax
    

    mov al, '1'
    call .printch


    mov ax, 0				; reset disk function
    int 13h					; call BIOS interrupt
    jc .disk_reset_error

    mov ax, 0
    mov es, ax
	mov bx, KERNEL_START		; target
 
	mov ah, 2			; read sectors function
	mov al, byte [KERNEL_SECT_COUNT]	        ; number of sectors
	mov ch, 0			; cylinder number
	mov dh, 0			; head number
	mov cl, byte [KERNEL_SECT_START]		; starting sector number
    ; dl implicit from bios
	int 13h				; call BIOS interrupt
	jc .disk_read_error



    mov al, '2'
    call .printch

    mov ax,0
    mov es, ax
    mov al, [es:XANADU]
    call .printch


    jmp $
    hlt

    lgdt [INITIAL_GDT_END_DESCRIPTOR]
    mov eax, cr0 
    or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov cr0, eax


   ; Reload CS register containing code selector:
   JMP   CODE_SEL:.reload_CS ; 0x08 is a stand-in for your code segment
.reload_CS:
   ; Reload data segment registers:
   MOV   AX, DATA_SEL ; 0x10 is a stand-in for your data segment
   MOV   DS, AX
   MOV   ES, AX
   MOV   FS, AX
   MOV   GS, AX
   MOV   SS, AX
   MOV   SP, STACK_START_2



    jmp $

    ; jump to next section
    jmp CODE_SEL:KERNEL_START


.disk_reset_error:
    mov al, 'D'
    jmp .err

.disk_read_error:
    mov al, 'R'
    jmp .err

.err:
    call .printch
    jmp $


; char in AL
.printch:
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


times 507-($-$$) db 0
XANADU db 'J'
KERNEL_SECT_START db 2
KERNEL_SECT_COUNT db 32
db 0x55, 0xaa
