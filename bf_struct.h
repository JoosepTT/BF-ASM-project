#ifndef BF_STRUCT_H
#define BF_STRUCT_H

#include <stdio.h>

typedef struct BF_instruction BF_instruction;

// põhiline andmestruktuur
struct BF_instruction {

    void (*printAsm)(BF_instruction *self, int *index, FILE *out); // funktsiooni pointer
    
    union { // mälu säästmiseks jagavad kõik väljad sama mäluala, kuid korraga kasutatakse ainult ühte
        int value; // +/-/move/add väärtus
        int loop_forward; // [ korral hüpe edasi
        int loop_back; // ] korral hüpe tagasi
    };
};

// konstruktorid, mis loovad juhised
BF_instruction* new_inc(int count);
BF_instruction* new_dec(int count);
BF_instruction* new_right(int count);
BF_instruction* new_left(int count);
BF_instruction* new_print(void);
BF_instruction* new_input(void);
BF_instruction* new_loop_start(void);
BF_instruction* new_loop_end(void);

// mälu vabastamine
void bf_instruction_free(BF_instruction *instr);

#endif