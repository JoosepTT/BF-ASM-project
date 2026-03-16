// BF faili parser ja kogu protsessi juhtfail

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf_struct.h"

#define MAX_INSTRUCTIONS 32768

// päise loomine
static void print_header(FILE *out) {
    fprintf(out, "; Brainfuck programm kompilleeritud %s\n", __DATE__);
    fprintf(out, "global main\n");
    fprintf(out, "extern mem_init\n");
    fprintf(out, "extern mem_cleanup\n");
    fprintf(out, "extern mem_inc\nextern mem_dec\n");
    fprintf(out, "extern mem_add\nextern mem_move\n");
    fprintf(out, "extern mem_right\nextern mem_left\n");
    fprintf(out, "extern mem_get\nextern mem_set\n");
    fprintf(out, "extern putchar\nextern getchar\nextern exit\n\n");
    fprintf(out, "section .text\n");
    fprintf(out, "main:\n");
    fprintf(out, "    call mem_init\n\n");
}

// lõpuosa
static void print_footer(FILE *out) {
    fprintf(out, "\n    ; Programm lõppetas töö\n");
    fprintf(out, "    call mem_cleanup\n"); // dünaamilise mälu vabastamine
    fprintf(out, "    xor eax, eax\n");
    fprintf(out, "    ret\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Kasutus: %s <sisend.bf> <väljund.asm>\n", argv[0]); // juhend
        return 1;
    }

    // faili avamine
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Sisendfaili ei saanud avada!");
        return 1;
    }

    // kogu fail sisselugemine
    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    fseek(in, 0, SEEK_SET);

    if (size <= 0) {
        fprintf(stderr, "Tühi või vigane sisendfail\n");
        fclose(in);
        return 1;
    }

    char *code = malloc(size + 1); // mälu eraldamine BF koodi tarbeks
    if (!code) {
        perror("malloc ebaõnnestus");
        fclose(in);
        return 1;
    }

    // faili lugemine
    size_t read_bytes = fread(code, 1, size, in);
    if (read_bytes != (size_t)size) {
        fprintf(stderr, "Faili lugemine ebaõnnestus (loe %zu / %ld baiti)\n", read_bytes, size);
        free(code);
        fclose(in);
        return 1;
    }
    code[size] = '\0';
    fclose(in);


    // BF juhiste kogumine:
    BF_instruction *instructions[MAX_INSTRUCTIONS] = {0};
    int instructions_count = 0;

    int loop_stack[1024];
    int loop_depth = 0;

    // BF koodi parsimine ühe sümboli kaupa:
    int i = 0;
    while (code[i]) {
        char c = code[i];
        i++;

        if (c == '+') {
            int cnt = 1;
            while (code[i] == '+') { cnt++; i++; }
            if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
            instructions[instructions_count++] = new_inc(cnt);
            continue;
        }
        if (c == '-') {
            int cnt = 1;
            while (code[i] == '-') { cnt++; i++; }
            if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
            instructions[instructions_count++] = new_dec(cnt);
            continue;
        }
        if (c == '>') {
            int cnt = 1;
            while (code[i] == '>') { cnt++; i++; }
            if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
            instructions[instructions_count++] = new_right(cnt);
            continue;
        }
        if (c == '<') {
            int cnt = 1;
            while (code[i] == '<') { cnt++; i++; }
            if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
            instructions[instructions_count++] = new_left(cnt);
            continue;
        }


        // üksikmärkide käsitlus:
        switch (c) {
            case '.':
                if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
                instructions[instructions_count++] = new_print();
                break;
            case ',':
                if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
                instructions[instructions_count++] = new_input();
                break;
            case '[':
                if (loop_depth >= 1024) {
                    fprintf(stderr, "Liiga sügav tsükkel!\n");
                    goto cleanup;
                }
                if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
                instructions[instructions_count] = new_loop_start();
                loop_stack[loop_depth++] = instructions_count++;
                break;
            case ']':
                if (loop_depth == 0) {
                    fprintf(stderr, "Sulgemata tsükkel! ]\n");
                    goto cleanup;
                }
                int open = loop_stack[--loop_depth];
                if (instructions_count >= MAX_INSTRUCTIONS) goto overflow;
                instructions[instructions_count] = new_loop_end();
                instructions[instructions_count]->loop_back = open;
                instructions[open]->loop_forward = instructions_count;
                instructions_count++;
                break;
            default:
                // BF-le tundmatute märkide ignoreerimine
                break;
        }
    }

    // tsüklite kontroll
    if (loop_depth > 0) {
        fprintf(stderr, "Avamata tsükkel! [\n");
        goto cleanup;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        perror("Ei saanud luua väljundfaili");
        goto cleanup;
    }

    print_header(out);

    int idx = 0;
    for (int j = 0; j < instructions_count; j++) {
        if (instructions[j]) {
            instructions[j]->printAsm(instructions[j], &idx, out);
        }
    }

    print_footer(out);
    fclose(out);

    printf("Genereeritud: %s\n", argv[2]);

    goto cleanup;

overflow:
    fprintf(stderr, "Liiga palju instruktsioone (max %d)\n", MAX_INSTRUCTIONS);


// mälu vabastamine:
cleanup:
    for (int j = 0; j < instructions_count; j++) {
        if (instructions[j]) bf_instruction_free(instructions[j]);
    }
    free(code);
    return (out == NULL || instructions_count >= MAX_INSTRUCTIONS) ? 1 : 0;
}