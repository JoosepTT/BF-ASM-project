; dünaamilise mälu implementatsioon asmis, mis sisaldab kõiki põhioperatsioone

; globaalsed muutujad
section .data
    tape_ptr      dd 0 ; pointer, mis näitab BF mälu algust
    tape_capacity dd 30000 ; mäluriba praegune suurus baitides
    ptr           dd 0 ; andmepointer

    msg_realloc_fail db "Realloc ebaõnnestus!", 10, 0

section .text

; standardteegi funktsioonid:
extern malloc
extern realloc
extern free
extern memset
extern exit
extern puts

; sümbolite eksport linkerile:
global mem_init
global mem_cleanup
global mem_inc
global mem_dec
global mem_add
global mem_right
global mem_left
global mem_move
global mem_get
global mem_set


;============================================
; void mem_init(void)
mem_init: ; mälu initsialiseerimine
    push 30000
    call malloc
    add  esp, 4
    test eax, eax
    jz   mem_init_alloc_failed

    mov  [tape_ptr], eax

    push 30000
    push 0
    push eax
    call memset
    add  esp, 12
    ret

mem_init_alloc_failed:
    push 1
    call exit

;===================================================
; void mem_cleanup(void)
mem_cleanup:
    cmp  dword [tape_ptr], 0
    je   mem_cleanup_done

    push dword [tape_ptr]
    call free
    add  esp, 4

    ; kogu oleku algväärtustamine
    mov  dword [tape_ptr], 0
    mov  dword [tape_capacity], 0
    mov  dword [ptr], 0

mem_cleanup_done:
    ret

;======================================
; void mem_inc(void)
mem_inc:
    mov  eax, [tape_ptr]
    mov  ebx, [ptr]
    inc  byte [eax + ebx]
    ret

;=======================================
; void mem_dec(void)
mem_dec:
    mov  eax, [tape_ptr]
    mov  ebx, [ptr]
    dec  byte [eax + ebx]
    ret

;======================================
; void mem_add(int v)
mem_add:
    mov  ecx, [esp+4]
    mov  eax, [tape_ptr]
    mov  ebx, [ptr]
    add  byte [eax + ebx], cl
    ret

;======================================
; void mem_right(void) (nihutamine)
mem_right:
    inc  dword [ptr]

    mov  eax, [ptr]
    cmp  eax, [tape_capacity]
    jb   mem_right_done

    ; mälu suurendamine
    mov  edx, [tape_capacity]
    shl  dword [tape_capacity], 1
    mov  ebx, [tape_capacity]

    push ebx
    push dword [tape_ptr]
    call realloc
    add  esp, 8

    test eax, eax
    jz   mem_right_realloc_failed

    mov  [tape_ptr], eax

    mov  ecx, ebx
    sub  ecx, edx
    push ecx
    push 0
    lea  edi, [eax + edx]
    push edi
    call memset
    add  esp, 12

mem_right_done:
    ret

mem_right_realloc_failed:
    push msg_realloc_fail
    call puts
    add  esp, 4
    push 1
    call exit


;=====================================
; void mem_left(void)
mem_left:
    ; pointeri hoidmine positiivsete väärtuste piirides
    cmp  dword [ptr], 0
    jbe  mem_left_done
    dec  dword [ptr]
mem_left_done:
    ret

;=====================================
; void mem_move(int delta)
mem_move:
    mov  ecx, [esp+4]
    add  [ptr], ecx

    cmp  dword [ptr], 0
    jge  mem_move_check_right
    mov  dword [ptr], 0
    jmp  mem_move_done

mem_move_check_right:
    mov  eax, [ptr]
    cmp  eax, [tape_capacity]
    jb   mem_move_done

mem_move_grow_loop:
    mov  edx, [tape_capacity]
    shl  edx, 1
    mov  [tape_capacity], edx
    cmp  eax, edx
    jae  mem_move_grow_loop

    push edx
    push dword [tape_ptr]
    call realloc
    add  esp, 8

    test eax, eax
    jz   mem_move_realloc_failed

    mov  [tape_ptr], eax

    push edx
    push 0
    push eax
    call memset
    add  esp, 12

mem_move_done:
    ret

mem_move_realloc_failed:
    push msg_realloc_fail
    call puts
    add  esp, 4
    push 1
    call exit

;======================================
; int mem_get(void)
mem_get:
    mov  eax, [tape_ptr]
    mov  ebx, [ptr]
    movzx eax, byte [eax + ebx]
    ret

;=======================================
; void mem_set(int v)
mem_set:
    mov  ecx, [esp+4]
    mov  eax, [tape_ptr]
    mov  ebx, [ptr]
    mov  [eax + ebx], cl
    ret