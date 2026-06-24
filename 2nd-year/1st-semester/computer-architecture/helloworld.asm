name "hola_mundo"
org 100h            ; Indica que el programa se carga en el offset 100h (Formato .COM)

jmp inicio          ; Salto incondicional para evitar ejecutar las variables como codigo

; SEGMENTO DE DATOS

; db = Define Byte. El simbolo '$' es el final obligatorio para la funcion 09h de la INT 21h.
msg_hola  db "Hello, World!", 0Dh, 0Ah, "$" 


; SEGMENTO DE CODIGO

inicio:
    ; 1. Configurar el Registro de Segmento de Datos (DS)
    ; Aunque en archivos .COM CS y DS suelen apuntar al mismo sitio, 
    ; es una excelente practica de arquitectura inicializar DS explicitamente.
    mov ax, cs      
    mov ds, ax      

    ; 2. Preparar el puntero al mensaje
    ; Cargamos el offset (la direccion de memoria inicial) de la cadena en DX.
    lea dx, msg_hola  ; Tambien valido: mov dx, offset msg_hola

    ; 3. Llamar al servicio de impresion de MS-DOS
    mov ah, 09h     ; Funcion 09h: Imprimir cadena de caracteres en pantalla
    int 21h         ; Interrupcion de servicios de DOS (ejecuta la funcion en AH)

    ; 4. Terminar el programa de forma segura
    mov ah, 4Ch     ; Funcion 4Ch: Devolver el control al sistema operativo
    mov al, 0       ; Codigo de retorno 0 (Ejecucion exitosa)
    int 21h         ; Interrupcion de servicios de DOS

end                 ; Fin del archivo para el compilador