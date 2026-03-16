// määrab, kuidas teisendada iga BF instruktsioon masinkoodiks
// iga BF-i käsk on objekt, mis oskab genereerida masinkoodi

#include "bf_struct.h"
#include <stdlib.h>
#include <stdio.h>

// +++
static void print_inc(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "    ; +%d\n", self->value);
    if (self->value == 1) {
        fprintf(out, "    call mem_inc\n");
    } else {
        fprintf(out, "    push %d\n", self->value);
        fprintf(out, "    call mem_add\n");
        fprintf(out, "    add esp, 4\n");
    }
    (*index)++; // iga BF juhis suurendab unikaalsete siltide loendurit
}

// BF juhiste objekti loomine
BF_instruction* new_inc(int count) {
    BF_instruction *i = malloc(sizeof(BF_instruction));
    i->value = count;
    i->printAsm = print_inc;
    return i;
}

// ---
static void print_dec(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "    ; -%d\n", self->value);
    if (self->value == 1) {
        fprintf(out, "    call mem_dec\n");
    } else {
        fprintf(out, "    push %d\n", -self->value);
        fprintf(out, "    call mem_add\n");
        fprintf(out, "    add esp, 4\n");
    }
    (*index)++;
}

BF_instruction* new_dec(int count) {
    BF_instruction *i = malloc(sizeof(BF_instruction));

    i->value = count;
    i->printAsm = print_dec;
    return i;
}

// >>>
static void print_right(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "    ; >%d\n", self->value);
    if (self->value == 1) { //  ühe sammu korral
        fprintf(out, "    call mem_right\n");
    } else { // kui samme on rohkem
        fprintf(out, "    push %d\n", self->value);
        fprintf(out, "    call mem_move\n");
        fprintf(out, "    add esp, 4\n"); // vajadusel mälu laiendamine
    }
    (*index)++;
}

BF_instruction* new_right(int count) {
    BF_instruction *i = malloc(sizeof(BF_instruction));
    i->value = count;
    i->printAsm = print_right;

    return i;
}

// <<<
static void print_left(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "    ; <%d\n", self->value);
    if (self->value == 1) {
        fprintf(out, "    call mem_left\n");
    } else {
        fprintf(out, "    push %d\n", -self->value);
        fprintf(out, "    call mem_move\n");
        fprintf(out, "    add esp, 4\n");
    }
    (*index)++;
}

BF_instruction* new_left(int count) {
    BF_instruction *i = malloc(sizeof(BF_instruction));
    i->value = count;
    i->printAsm = print_left;
    return i;
}

// . (väljund)
static void print_dot(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "    ; .\n");
    fprintf(out, "    call mem_get\n");
    fprintf(out, "    push eax\n");
    fprintf(out, "    call putchar\n");
    fprintf(out, "    add esp, 4\n");
    (*index)++;
}

BF_instruction* new_print(void) {
    BF_instruction *i = malloc(sizeof(BF_instruction));
    i->printAsm = print_dot;
    return i;
}

// ,
static void print_comma(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "    ; ,\n");
    fprintf(out, "    call getchar\n");
    fprintf(out, "    push eax\n");
    fprintf(out, "    call mem_set\n");
    fprintf(out, "    add esp, 4\n");
    (*index)++;
}

BF_instruction* new_input(void) {

    BF_instruction *i = malloc(sizeof(BF_instruction));
    i->printAsm = print_comma;
    return i;
}

// [
static void print_loop_start(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "loop_start_%d:\n", *index);
    fprintf(out, "    call mem_get\n");
    fprintf(out, "    test eax, eax\n");
    fprintf(out, "    jz loop_end_%d\n", self->loop_forward);
    (*index)++;
}

BF_instruction* new_loop_start(void) {
    BF_instruction *i = malloc(sizeof(BF_instruction));
    i->printAsm = print_loop_start;
    return i;
}

// ]
static void print_loop_end(BF_instruction *self, int *index, FILE *out) {
    fprintf(out, "    jmp loop_start_%d\n", self->loop_back);
    fprintf(out, "loop_end_%d:\n", *index);
    (*index)++;
}

BF_instruction* new_loop_end(void) {
    BF_instruction *i = malloc(sizeof(BF_instruction));
    i->printAsm = print_loop_end;
    return i;
}


// jällegi mälu vabastamine
void bf_instruction_free(BF_instruction *instr) {
    free(instr);
}