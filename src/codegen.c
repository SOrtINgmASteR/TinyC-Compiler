#include "codegen.h"

#include <string.h>

static const char *relop_to_set(const char *op) {
    if (strcmp(op, "==") == 0) return "SETE";
    if (strcmp(op, "!=") == 0) return "SETNE";
    if (strcmp(op, "<") == 0) return "SETL";
    if (strcmp(op, ">") == 0) return "SETG";
    if (strcmp(op, "<=") == 0) return "SETLE";
    if (strcmp(op, ">=") == 0) return "SETGE";
    return "SET";
}

void codegen_generate(const TacInstr *head, FILE *out) {
    const TacInstr *cur = head;
    if (!out) return;

    while (cur) {
        if (strcmp(cur->op, "label") == 0) {
            fprintf(out, "%s:\n", cur->result);
        } else if (strcmp(cur->op, "goto") == 0) {
            fprintf(out, "JMP %s\n", cur->result);
        } else if (strcmp(cur->op, "ifz") == 0) {
            fprintf(out, "CMP %s, 0\n", cur->arg1);
            fprintf(out, "JE %s\n", cur->result);
        } else if (strcmp(cur->op, "print") == 0) {
            fprintf(out, "PRINT %s\n", cur->arg1);
        } else if (strcmp(cur->op, "return") == 0) {
            if (cur->arg1) {
                fprintf(out, "RET %s\n", cur->arg1);
            } else {
                fprintf(out, "RET\n");
            }
        } else if (strcmp(cur->op, "=") == 0) {
            fprintf(out, "MOV %s, %s\n", cur->result, cur->arg1);
        } else if (strcmp(cur->op, "+") == 0 || strcmp(cur->op, "-") == 0 ||
                   strcmp(cur->op, "*") == 0 || strcmp(cur->op, "/") == 0 ||
                   strcmp(cur->op, "%") == 0) {
            fprintf(out, "MOV R1, %s\n", cur->arg1);
            if (strcmp(cur->op, "+") == 0) fprintf(out, "ADD R1, %s\n", cur->arg2);
            else if (strcmp(cur->op, "-") == 0) fprintf(out, "SUB R1, %s\n", cur->arg2);
            else if (strcmp(cur->op, "*") == 0) fprintf(out, "MUL R1, %s\n", cur->arg2);
            else if (strcmp(cur->op, "/") == 0) fprintf(out, "DIV R1, %s\n", cur->arg2);
            else if (strcmp(cur->op, "%") == 0) fprintf(out, "MOD R1, %s\n", cur->arg2);
            fprintf(out, "MOV %s, R1\n", cur->result);
        } else if (strcmp(cur->op, "==") == 0 || strcmp(cur->op, "!=") == 0 ||
                   strcmp(cur->op, "<") == 0 || strcmp(cur->op, ">") == 0 ||
                   strcmp(cur->op, "<=") == 0 || strcmp(cur->op, ">=") == 0) {
            fprintf(out, "CMP %s, %s\n", cur->arg1, cur->arg2);
            fprintf(out, "%s %s\n", relop_to_set(cur->op), cur->result);
        }
        cur = cur->next;
    }
}
