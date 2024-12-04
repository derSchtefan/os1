BITS 16

STACK_END equ 0x0500
STACK_START equ 0x7000
BOOT2_LOC equ 0x7E00
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

    mov al, '1'
    call .printch


    mov ax, 0				; reset disk function
    int 13h					; call BIOS interrupt
    jc .disk_reset_error

    ; es implicit 0
	mov bx, BOOT2_LOC		; index 0
 
	mov ah, 2			; read sectors function
	mov al, SECT_COUNT_B2	; number of sectors
	mov ch, 0			; cylinder number
	mov dh, 0			; head number
	mov cl, 2			; starting sector number
    ; dl implicit from bios
	int 13h				; call BIOS interrupt
	jc .disk_read_error




    ; jump to next section
    jmp 0x00:BOOT2_LOC


; Define segment selectors
CODE_SEG equ 8
DATA_SEG equ 16

; Define GDT entries
.INITIAL_GDT_START:
NULL_SEL    equ $ - INITIAL_GDT_START
    db 0,0,0,0             ; Null segment
CODE_SEL    equ $ - INITIAL_GDT_START
    db 0xFF,0xFF,0,0       ; Code segment
    db 0,0,0,0
    db 0x9A,0xCF,0,0       ; Present, executable, readable
DATA_SEL    equ $ - INITIAL_GDT_START
    db 0xFF,0xFF,0,0       ; Data segment
    db 0,0,0,0
    db 0x92,0xCF,0,0       ; Present, writeable, readable
.INITIAL_GDT_END:
.INITIAL_GDT_END_DESCRIPTOR:
    dw INITIAL_GDT_END - INITIAL_GDT_START - 1
    dd INITIAL_GDT_START


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

times 508-($-$$) db 0
.KERNEL_SECT_START db 1 + 1 + SECT_COUNT_B2
.KERNEL_SECT_COUNT db 255
db 0x55, 0xaa

section BOOT2 follows=BOOT vstart=BOOT2_LOC
    mov al, '2'
    call .printch


    jmp $
    hlt


.SECT_PAD equ 512 - (($-$$) %% 512)
times (.SECT_PAD == 512 ? 0 : .SECT_PAD) db 0
SECT_COUNT_B2 equ ($-$$)/512