#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include "ast.h"

typedef struct TacInstr {
    char *op;
    char *arg1;
    char *arg2;
    char *result;
    struct TacInstr *next;
} TacInstr;

void tac_reset(void);
void tac_generate(const AstNode *root);
void tac_print(FILE *out);
const TacInstr *tac_get_head(void);

#endif
