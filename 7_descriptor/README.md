# Решение задачи по составлению дескриптора

Программа `main.py` составляет дескрипторы памяти по 3-м примерам, взятым из [этой статьи](https://fat-crocodile.narod.ru/protected/02_segments.html):

> Дескриптор сегмента данных, размер сегмента 4 Гб, базовый адрес 0, Read/Write:
>
> ```asm
>     db      0FFh        ; Segment Limit
>     db      0FFh 
>     db      0           ; base address
>     db      0 
>     db      0 
>     db      10010010b   ; 1001, C/D – 0, 0, R/W – 1, 0
>     db      10001111b   ; G - 1, 000, Limit - 1111
>     db      0           ; base address
> ```
> 
> Дескриптор сегмента данных, размер сегмента 64 Кб, базовый адрес 0, Read/Write:
> 
> ```asm
>     db      0FFh        ; Segment Limit
>     db      0FFh 
>     db      0           ; base address
>     db      0 
>     db      0 
>     db      10010010b   ; 1001, C/D – 0, 0, R/W – 1, 0
>     db      00000000b   ; G - 0, 000, Limit - 0000
>     db      0           ; base address
> ```
> 
> Дескриптор сегмента кода, размер сегмента 64 Кб, базовый адрес 12345678h, Execute/Read:
> 
> ```asm
>     db      0FFh        ; Segment Limit
>     db      0FFh 
>     db      78h         ; base address
>     db      56h
>     db      34h
>     db      10011010b   ; 1001, C/D – 1, 0, R/W – 1, 0
>     db      00000000b   ; G - 0, 000, Limit - 0000
>     db      12h         ; base address
> ```

Пример вывода программы:

```
Example 1: FF FF 00 00 00 92 8F 00
Example 2: FF FF 00 00 00 92 00 00
Example 3: FF FF 78 56 34 9A 00 12
```
