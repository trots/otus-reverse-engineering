use16
OldPlace EQU 0x7c00
NewPlace EQU 0x600
delta EQU newOffset-start

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
        rep movsb		;	ds:si ==> es:di

        push es
        push NewPlace + delta
        retf
		
newOffset:  
	
org NewPlace + delta
	
        mov bx, cs
        mov ds, bx
        mov es, bx
        xor bx, bx
		xchg bx, bx
        ;lea si, [Message - 0x7c00 + 0x600]
		lea si, [Message]
@@:             
        mov ah, 0Eh
        lodsb        
		test al, al
        jz @F
        int 10h         
        jmp @B        
@@:
        mov ax, 0x0201
        mov dx, 0x80
        mov cx, 1
		
        mov bx, 0x7c00		
        int 13h 
                
		xor ax, ax
        int 16h

        push 0
        push 7c00h
        retf

Message db 'Hello!!!',0
times 510-($-$$ + delta) db 0
db 0x55, 0xaa