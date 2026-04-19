# Решение задачи на установку пароля на этапе загрузки MBR

Решение `mbr-password.asm` создано на основе исходного `mbr.asm`. Сначала выполняется загрузка кастомного MBR и при вводе правильного пароля управление передается на оригинальный MBR.

Сборка `mbr.bin` через fasm:
```sh
fasm mbr-password.asm mbr.bin
```

Пример запуска кастомного MBR через Bochs:

https://github.com/user-attachments/assets/1c46f2d9-1a6d-46f6-857d-9cdd6f48dc52


