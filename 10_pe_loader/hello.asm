format PE GUI 4.0 at 0
entry start

include 'win32a.inc'

section '.text' code readable executable

start:
    mov     eax, [ptr_caption]
    mov     ebx, [ptr_message]

    push    0
    push    eax
    push    ebx
    push    0
    call    [MessageBoxA]
    
    push    0
    call    [ExitProcess]

section '.data' data readable writeable

    message db 'Hello!', 0
    caption db 'PE Loader Test', 0

    ptr_message  dd message
    ptr_caption  dd caption

section '.idata' import data readable writeable

    library kernel32, 'kernel32.dll', \
            user32,   'user32.dll'
    
    import kernel32, \
           ExitProcess, 'ExitProcess'
    
    import user32, \
           MessageBoxA, 'MessageBoxA'

section '.reloc' data readable discardable

    data fixups
    end data
