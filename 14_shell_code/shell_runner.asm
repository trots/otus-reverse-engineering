format PE GUI 4.0
entry start

;include '../INCLUDE/win32a.inc'

section '.shell' code readable writeable executable
shellStart:
	file "calc.bin"
shellEnd:

section '.text' code readable executable
start:	
	nop
	jmp shellStart
	int 3
buf dd 0

;library kernel32,'KERNEL32.DLL',\
;        user32,'USER32.DLL'

;import kernel32,\
;       ExitProcess,'ExitProcess', \
;        VirtualAlloc,'VirtualAlloc'

