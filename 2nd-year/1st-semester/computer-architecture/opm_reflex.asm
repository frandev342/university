name "opm_reflex"
org 100h

jmp inicio_programa    ; SALTO INCONDICIONAL: Esquiva los textos para no ejecutarlos como codigo.

; =======================================================
; --- SEGMENTO DE DATOS (VARIABLES Y MENSAJES) ---
; =======================================================

msg_titulo     db 0Dh, 0Ah, "=======================================", 0Dh, 0Ah
               db "     DUELO DE MARTILLAZOS: SPEED REFLEX", 0Dh, 0Ah                           
               db "=======================================", 0Dh, 0Ah, "$"
msg_play       db 0Dh, 0Ah, "          [ PRESIONA ENTER ]           ", 0Dh, 0Ah, "$"
msg_reglas     db 0Dh, 0Ah, "----------- REGLAS DEL JUEGO -----------", 0Dh, 0Ah
               db "Cuando aparezca '!!! MARTILLAZO !!!',", 0Dh, 0Ah
               db "presiona una tecla antes de 5 ticks.", 0Dh, 0Ah
               db "Si eres rapido, golpeas a la CPU.", 0Dh, 0Ah
               db "Si eres lento, la CPU te golpeara a ti.", 0Dh, 0Ah
               db 0Dh, 0Ah, "Presiona una tecla para continuar...$"
msg_pedir_v    db 0Dh, 0Ah, 0Dh, 0Ah, "Definir victorias para ganar (1 a 9): $"
msg_ronda      db 0Dh, 0Ah, "---------------------------------------", 0Dh, 0Ah
               db "�Prepara el martillo! Espera...$"
msg_ya         db 0Dh, 0Ah, "!!! MARTILLAZO !!! �PRESIONA YA!: $"
msg_golpe_user db 0Dh, 0Ah, ">> �PUM! Fuiste mas rapido. Golpeas a la CPU. <3$"
msg_golpe_cpu  db 0Dh, 0Ah, ">> �ZAS! Fuiste muy lento. La CPU te golpeo. :($ "
msg_marcador   db 0Dh, 0Ah, "MARCADOR -> TU: $"
msg_marcador_c db "  |  CPU: $"
msg_gana_part  db 0Dh, 0Ah, 0Dh, 0Ah, "=======================================", 0Dh, 0Ah
               db "     �VICTORIA TOTAL! ERES EL REY      ", 0Dh, 0Ah
               db "=======================================", 0Dh, 0Ah, "$"
msg_pierde_part db 0Dh, 0Ah, 0Dh, 0Ah, "=======================================", 0Dh, 0Ah
               db "     DERROTA. LA CPU TE DEJO K.O.     ", 0Dh, 0Ah
               db "=======================================", 0Dh, 0Ah, "$"
msg_reinicio   db 0Dh, 0Ah, "Presiona cualquier tecla para ir al menu...$"

victorias_req  db 0
score_user     db 0
score_cpu      db 0
tiempo_inicial dw 0

; =======================================================
; --- SEGMENTO DE CODIGO (LOGICA DEL JUEGO) ---
; =======================================================
inicio_programa:

menu_principal:
    mov score_user, 0 ; Resetea marcador del jugador.
    mov score_cpu, 0  ; Resetea marcador de la CPU.

    ; --- INT 10h / AH=00h: Limpia la pantalla cambiando el modo de video ---
    mov ah, 0         
    mov al, 3         ; Modo texto 80x25 (borra todo lo que habia antes).
    int 10h           

    ; --- INT 21h / AH=09h: Imprime el titulo (DX apunta al string que acaba en $) ---
    mov dx, offset msg_titulo 
    mov ah, 9                 
    int 21h                   

    mov dx, offset msg_play   
    mov ah, 9                 
    int 21h                   

esperar_play:
    ; --- INT 16h / AH=00h: Pausa el juego hasta que metas una tecla (Cae en AL) ---
    mov ah, 0         
    int 16h           
    cmp al, 0Dh       ; Compara si la tecla en AL es ENTER (0Dh en ASCII).
    jne esperar_play  ; SALTO CONDICIONAL: Si NO es ENTER, rebota arriba y sigue esperando.

pantalla_explicacion:
    mov ah, 0         ; INT 10h / AH=00h: Limpia pantalla otra vez.
    mov al, 3         
    int 10h           

    mov dx, offset msg_reglas ; INT 21h / AH=09h: Muestra instrucciones.
    mov ah, 9                 
    int 21h                   

    mov ah, 0         ; INT 16h / AH=00h: Pausa de lectura hasta presionar cualquier tecla.
    int 16h           

pantalla_configuracion:
    mov dx, offset msg_pedir_v ; INT 21h / AH=09h: Pide cuantas victorias definen el juego.
    mov ah, 9                  
    int 21h                    

pedir_numero:
    ; --- INT 21h / AH=01h: Lee una tecla del teclado y la pinta en pantalla (Eco) ---
    mov ah, 1         
    int 21h           ; El caracter ASCII se guarda en AL.
    cmp al, '1'       
    jb pedir_numero   ; SALTO CONDICIONAL: Si es MENOR que el caracter '1', lo ignora y vuelve a pedir.
    cmp al, '9'       
    ja pedir_numero   ; SALTO CONDICIONAL: Si es MAYOR que el caracter '9', lo ignora y vuelve a pedir.

    sub al, '0'       ; Truco: Resta 30h al ASCII para tener el numero entero real en AL.
    mov victorias_req, al ; Guarda la meta de puntos.

; -------------------------------------------------------
; --- BUCLE PRINCIPAL DE LAS RONDAS ---
; -------------------------------------------------------
gameloop:
    mov al, score_user
    cmp al, victorias_req
    je usuario_gana_juego ; SALTO CONDICIONAL: Si tus puntos son IGUALES a la meta, ganas la partida.

    mov al, score_cpu
    cmp al, victorias_req
    je cpu_gana_juego     ; SALTO CONDICIONAL: Si la CPU te iguala la meta, pierdes la partida.

    mov dx, offset msg_marcador ; INT 21h / AH=09h: Imprime la etiqueta del marcador.
    mov ah, 9
    int 21h

    mov al, score_user
    add al, '0'              ; Convierte tu numero a caracter ASCII para que se vea bien.
    ; --- INT 10h / AH=0Eh: Imprime el caracter que esta en AL (Modo Teletype) ---
    mov ah, 0Eh              
    int 10h                  

    mov dx, offset msg_marcador_c ; INT 21h / AH=09h: Imprime la mitad del marcador " | CPU: ".
    mov ah, 9
    int 21h

    mov al, score_cpu
    add al, '0'              ; Convierte numero de la CPU a ASCII.
    mov ah, 0Eh              ; INT 10h / AH=0Eh: Pinta los puntos de la CPU.
    int 10h                  

    mov dx, offset msg_ronda ; INT 21h / AH=09h: Dice "�Prepara el martillo! Espera...".
    mov ah, 9
    int 21h

    ; --- Bucle de retraso (Quema tiempo usando el registro CX) ---
    mov cx, 0500h     ; Carga 1280 ciclos en CX.
pausa_ronda:
    nop               ; No hace nada, solo gasta ciclos de reloj de la CPU.
    loop pausa_ronda  ; Resta 1 a CX. Si CX no es 0, salta arriba; si es 0, sale del bucle.

limpiar_buffer:
    ; --- INT 16h / AH=01h: Revisa el buffer del teclado sin bloquear (Anti-trampas) ---
    mov ah, 1         
    int 16h           ; Si NO hay teclas presionadas antes de tiempo, activa la bandera Zero (ZF=1).
    jz buffer_listo   ; SALTO CONDICIONAL: Si ZF=1 (Buffer limpio, juego legal), avanza.
    
    mov ah, 0         ; Si ZF=0, habia una tecla guardada. INT 16h/AH=00h la saca y la borra del buffer.
    int 16h           
    jmp limpiar_buffer ; SALTO INCONDICIONAL: Vuelve a revisar hasta dejar el buffer en blanco.

buffer_listo:
    ; --- INT 1Ah / AH=00h: Lee el reloj interno de la BIOS (Frecuencia de ~18.2 ticks por segundo) ---
    mov ah, 0         
    int 1Ah           ; Deja los ticks mas rapidos en el registro DX.
    mov tiempo_inicial, dx ; Guarda la marca de tiempo de salida.

    mov dx, offset msg_ya ; INT 21h / AH=09h: Da la se�al visual de golpe.
    mov ah, 9
    int 21h

    mov ah, 0         ; INT 16h / AH=00h: Congela todo esperando que el jugador presione una tecla.
    int 16h           

    mov ah, 0         ; INT 1Ah / AH=00h: Vuelve a checar el reloj en chinga justo cuando presionas la tecla.
    int 1Ah           
    
    sub dx, tiempo_inicial ; Resta: DX = Tiempo Final - Tiempo Inicial. Te da tus ticks netos de reaccion.

    cmp dx, 5         ; Compara si tardaste menos de 5 ticks (~275 milisegundos).
    jb user_golpea    ; SALTO CONDICIONAL: Si DX es MENOR que 5, reaccionaste rapido y vas a tu golpe.

cpu_golpea:
    inc score_cpu     ; Si fuiste lento, aumenta el marcador de la CPU.
    mov dx, offset msg_golpe_cpu ; INT 21h / AH=09h: Imprime que te pegaron.
    mov ah, 9
    int 21h
    jmp fin_ronda     ; SALTO INCONDICIONAL: Salta directo al cierre para no darle puntos al usuario.

user_golpea:
    inc score_user    ; Te anotas un punto.
    mov dx, offset msg_golpe_user ; INT 21h / AH=09h: Imprime tu martillazo exitoso.
    mov ah, 9
    int 21h

fin_ronda:
    ; --- Bucle de retraso optimizado para ver el resultado ---
    mov cx, 0100h
pausa_lectura:
    nop
    loop pausa_lectura ; Quema tiempo corto en CX antes de saltar.
    jmp gameloop   ; SALTO INCONDICIONAL: Regresa arriba para ver si alguien ya gano el juego.

; -------------------------------------------------------
; --- PANTALLAS DE DESENLACE FINAL ---
; -------------------------------------------------------
usuario_gana_juego:
    mov dx, offset msg_gana_part 
    jmp mostrar_fin   ; SALTO INCONDICIONAL: Salta a la rutina comun de impresion final.

cpu_gana_juego:
    mov dx, offset msg_pierde_part 

mostrar_fin:
    mov ah, 9         ; INT 21h / AH=09h: Pinta en pantalla el banner final cargado en DX.
    int 21h           

    mov dx, offset msg_reinicio ; INT 21h / AH=09h: Avisa como regresar.
    mov ah, 9
    int 21h

    mov ah, 0         ; INT 16h / AH=00h: Bloquea la consola hasta recibir una tecla de confirmacion.
    int 16h           
    jmp menu_principal ; SALTO INCONDICIONAL: Reinicia todo el programa desde cero.