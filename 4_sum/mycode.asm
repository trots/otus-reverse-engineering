
; You may customize this and other start-up templates; 
; The location of this template is c:\emu8086\inc\0_com_template.txt

org 100h

start:
    ; Initialize segment registers
    mov ax, cs
    mov ds, ax
    mov es, ax

    ; Print "start" message using BIOS INT 10h with function AH=0Eh (teletype function)   
    mov dx, offset msg_start
    call print

    ; Initialize buffer1 with zeros    
    mov cx, 512
    mov al, 0
    mov di, offset buffer1
    rep stosb
    
    ; Initialize buffer2 with zeros
    mov cx, 512
    mov di, offset buffer2
    rep stosb

    ; Initialize result_low with zeros    
    mov cx, 512
    mov di, offset result_low
    rep stosb

    ; Initialize result_high with zeros    
    mov cx, 512
    mov di, offset result_high
    rep stosb

    ; Print "reading" message    
    mov dx, offset msg_read
    call print
    
    ; Read sector 1 from FLOPPY using BIOS INT 13h with function AH=02h (Read Sectors)
    mov bx, offset buffer1
    mov al, 1
    mov ch, 0
    mov cl, 1
    mov dh, 0
    mov dl, 0
    call read_sector
    jc error ; If carry flag set, jump to error handler

    ; Read sector 2 from FLOPPY    
    mov bx, offset buffer2
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0
    call read_sector
    jc error
    
    ; Print "adding" message
    mov dx, offset msg_add
    call print
    
    ; Add two numbers
    call add_numbers
           
    ; Print "write" message
    mov dx, offset msg_write
    call print

    ; Write FLOPPY's sector 3 using BIOS INT 13h with function AH=03h (Write Sectors)     
    mov bx, offset result_low
    mov al, 1
    mov ch, 0
    mov cl, 3
    mov dh, 0
    mov dl, 0
    call write_sector
    jc error

    ; Write FLOPPY's sector 4    
    mov bx, offset result_high
    mov al, 1
    mov ch, 0
    mov cl, 4
    mov dh, 0
    mov dl, 0
    call write_sector
    jc error

    ; Print "done" message    
    mov dx, offset msg_done
    call print
    
    mov ax, 4C00h ; DOS function: exit program with return code 0
    int 21h ; Call DOS interrupt to terminate

error:
    ; Print "error" message
    mov dx, offset msg_error
    call print
    mov ax, 4C01h ; DOS function: exit program with return code 0
    int 21h

read_sector:
    mov ah, 02h
    int 13h
    ret

write_sector:
    mov ah, 03h
    int 13h
    ret

add_numbers:
    mov cx, 512 ; Set loop counter
    mov si, offset buffer1 ; Point si to the first number
    mov di, offset buffer2 ; Point di to the second number
    mov bx, offset result_low ; Point bx to result buffer
    clc ; Clear carry flag
    
add_loop:
    mov al, [si]
    adc al, [di] ; Add with carry falg (sozhenije stolbikom)
    mov [bx], al
    inc si
    inc di
    inc bx
    loop add_loop
    
    jnc no_carry ; If no final carry, skip next instruction
    mov byte ptr [result_high], 1
    
no_carry:
    ret

print:
    mov si, dx
    
print_loop:
    lodsb ; Load byte at [SI] into AL, increment SI
    cmp al, '$' ; Compare character with string terminator
    je print_end ; If '$' found, jump to end of print
    mov ah, 0Eh
    int 10h
    jmp print_loop
    
print_end:
    ret


buffer1     db 512 dup(0)   ; Buffer for sector 1
buffer2     db 512 dup(0)   ; Buffer for sector 2
result_low  db 512 dup(0)   ; Sector 3 - lsb
result_high db 512 dup(0)   ; Sector 4 - msb

msg_start   db 'Program started$'
msg_read    db 'Reading sectors...$'
msg_add     db 'Adding numbers...$'
msg_write   db 'Writing result...$'
msg_done    db 'Done!$'
msg_error   db 'Error!$'


end start ; Start is entry point
