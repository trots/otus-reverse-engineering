use16
org 0x7C00

start:
    ; Base initialization of a Real Mode
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print Real Mode message via BIOS interrupts
    mov si, msg_rm
    call print_string

    ; Switching to Protected Mode
    call switch_to_pm

; GDT & GDTR

gdt_start:
    dq 0                      ; Null descriptor (required for x86)

    ; Segment 1: Code (selector 0x08)
    ; Base=0, Limit=4GB, P=1, DPL=0, Code/Executable/Readable, 32-bit
    dw 0xFFFF                 ; Limit 0:15
    dw 0x0000                 ; Base 0:15
    db 0x00                   ; Base 16:23
    db 0x9A                   ; Access 10011010 (P=1, DPL=00, S=1, Type=1010)
    db 0xCF                   ; Flags 11001111 (G=1, D=1, Limit 16:19=1111)
    db 0x00                   ; Base 24:31

    ; Segment 2: Data (Selector 0x10)
    ; Base=0, Limit=4GB, P=1, DPL=0, Data/Readable/Writeable
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92                   ; Access: 10010010 (Type=0010 Data/RW)
    db 0xCF
    db 0x00

    ; Segment 3: Stack (selector 0x18)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
gdt_end:

gdtr:
    dw gdt_end - gdt_start - 1  ; Limit GDT
    dd gdt_start                ; Base GDT

; Print message in Real Mode

msg_rm db 'Real Mode OK', 0x0D, 0x0A, 0

print_string:
    push ax
.pm_loop:
    lodsb
    test al, al
    jz .pm_done
    mov ah, 0x0E
    int 0x10
    jmp .pm_loop
.pm_done:
    pop ax
    ret

; Switch to Protected Mode

switch_to_pm:
    cli                  ; Disable interrupts
    lgdt [gdtr]          ; Load GDTR
    mov eax, cr0
    or al, 1             ; Set PE-bit
    mov cr0, eax
    jmp 0x08:pmode_entry ; Far jump to load
    ret

; Protected Mode Code
use32
pmode_entry:
    ; Initialize segment registries
    mov ax, 0x10 ; Data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, 0x18 ; Stack selector
    mov ss, ax
    mov esp, 0x90000 ; Stack pointer

    ; Print 'P' symbol to VGA-memory in PM mode. BIOS interrupts are not available
    mov edi, 0xB8000
    mov word [edi], 0x0F50    ; 0x50 - 'P', 0x0F - white on black

    ; Halt
    cli ; Guarantee interrupt disabling
    hlt
    jmp $

; Boot Signature
times 510-($-$$) db 0
dw 0xAA55
