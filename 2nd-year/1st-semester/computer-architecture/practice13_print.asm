org 100h

jmp inicio

; --- Sección de Datos ---
; Insertamos 13 (regresar al inicio) y 10 (bajar de fila) antes del segundo texto
mensaje db "HELLO WORLD$", 13, 10, "THIS IS MY PROGRAM$"

inicio:
    mov dx, offset mensaje
    mov ah, 09h
    int 21h

    ; Salida limpia
    mov ah, 4Ch
    int 21h
ret