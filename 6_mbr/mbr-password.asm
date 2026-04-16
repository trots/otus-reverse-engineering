use16
OldPlace = 0x7c00
NewPlace = 0x600
delta = newOffset - start

org OldPlace
start:
        xor ax, ax              
        mov es, ax
        mov ds, ax
        mov ss, ax
        mov sp, 0xffff  

        mov ax, 3
        int 10h
                
        cld
        mov di, NewPlace
        mov si, OldPlace
        mov cx, 200h
        rep movsb               ; copy self to 0x600

        push es
        push NewPlace + delta
        retf                    ; move to copy
		
newOffset:  
org NewPlace + delta

        mov bx, cs
        mov ds, bx
        mov es, bx

        mov si, Prompt
        call PrintStr

.read_loop:
        xor cx, cx              ; cx contains length of entered password
        mov di, PassBuf

.input_char:
        xor ax, ax
        int 16h                 ; waiting for
        cmp al, 13              ; Enter?
        je .check_pass
        cmp al, 8               ; Backspace?
        je .backspace
        cmp al, 0               ; Ignore extended code (arrows, F1-F12...)
        je .input_char
        cmp cx, 16              ; buffer restriction
        jae .input_char
        mov [di], al
        inc di
        inc cx
        mov ah, 0Eh             ; print symbol to screen
        int 10h
        jmp .input_char

.backspace:
        jcxz .input_char
        dec di
        dec cx
        mov ax, 0E08h           ; backspace
        int 10h
        mov ax, 0E20h           ; space (to clear symbol)
        int 10h
        mov ax, 0E08h           ; backspace (back cursor)
        int 10h
        jmp .input_char

.check_pass:
        mov si, CRLF
        call PrintStr
        
        ; Clear keyboard buffer
        mov ah, 01h
        int 16h
        jz .no_more_keys
        xor ah, ah
        int 16h
        jmp .check_pass

.no_more_keys:
        mov si, PassBuf
        call CalcCRC32
        cmp edx, 0x0972D361 ; compare calculated CRC32 with hardcoded value
        je .load_os

        ; Wrong password
        mov si, ErrMsg
        call PrintStr
        mov si, CRLF
        call PrintStr
        mov si, Prompt
        call PrintStr
        jmp .read_loop          ; loop password request

.load_os:
        mov ax, 0x0201
        mov dx, 0x80
        mov cx, 1
        mov bx, 0x7c00		
        int 13h 

        push 0
        push 7c00h
        retf

PrintStr:
        lodsb
        test al, al
        jz .ret
        mov ah, 0Eh
        int 10h         
        jmp PrintStr        
.ret:   
        ret

CalcCRC32:
        push si bx cx
        mov edx, 0xFFFFFFFF 
        jcxz .crc_done
.crc_char:
        xor eax, eax
        lodsb
        xor dl, al
        mov bx, 8
.crc_bit:
        shr edx, 1
        jnc .crc_next
        xor edx, 0xEDB88320   ; CRC-32/ISO-3309
.crc_next:
        dec bx
        jnz .crc_bit
        loop .crc_char
.crc_done:
        not edx
        pop cx bx si
        ret

Prompt db 'Password: ',0
ErrMsg db 'Wrong password!',0
CRLF   db 13,10,0
PassBuf: times 16 db 0
HexBuf: times 9 db 0

times 510-($-$$+delta) db 0
db 0x55, 0xaa
