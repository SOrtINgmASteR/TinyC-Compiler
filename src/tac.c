#include "tac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static TacInstr *g_head = NULL;
static TacInstr *g_tail = NULL;
static int g_temp_count = 0;
static int g_label_count = 0;

static char *dup_text(const char *s) {
    return s ? strdup(s) : NULL;
}

static TacInstr *tac_add(const char *op, const char *arg1, const char *arg2, const char *result) {
    TacInstr *ins = (TacInstr *)malloc(sizeof(TacInstr));
    if (!ins) return NULL;
    ins->op = dup_text(op);
    ins->arg1 = dup_text(arg1);
    ins->arg2 = dup_text(arg2);
    ins->result = dup_text(result);
    ins->next = NULL;

    if (!g_head) {
        g_head = g_tail = ins;
    } else {
        g_tail->next = ins;
        g_tail = ins;
    }
    return ins;
}

static char *new_temp(void) {
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", ++g_temp_count);
    return dup_text(buf);
}

static char *new_label(void) {
    char buf[32];
    snprintf(buf, sizeof(buf), "L%d", ++g_label_count);
    return dup_text(buf);
}

static char *gen_expr(const AstNode *node) {
    if (!node) return NULL;

    switch (node->kind) {
        case AST_IDENT:
        case AST_INT:
        case AST_FLOAT:
        case AST_STRING:
        case AST_CHAR:
            return dup_text(node->text ? node->text : "");
        case AST_BINOP: {
            char *left = gen_expr(node->left);
            char *right = gen_expr(node->right);
            char *tmp = new_temp();
            tac_add(node->text, left, right, tmp);
            free(left);
            free(right);
            return tmp;
        }
        default:
            return NULL;
    }
}

static void gen_stmt(const AstNode *node) {
    if (!node) return;

    if (node->kind == AST_STMT_LIST) {
        gen_stmt(node->left);
        gen_stmt(node->right);
        return;
    }

    switch (node->kind) {
        case AST_FUNCTION:
            tac_add("label", NULL, NULL, node->text);
            gen_stmt(node->left);
            break;
        case AST_DECL:
            break;
        case AST_ASSIGN: {
            char *rhs = gen_expr(node->right);
            if (node->left && node->left->text) {
                tac_add("=", rhs, NULL, node->left->text);
            }
            free(rhs);
            break;
        }
        case AST_PRINT: {
            char *arg = gen_expr(node->left);
            tac_add("print", arg, NULL, NULL);
            free(arg);
            break;
        }
        case AST_RETURN: {
            char *arg = gen_expr(node->left);
            tac_add("return", arg, NULL, NULL);
            free(arg);
            break;
        }
        case AST_IF: {
            char *cond = gen_expr(node->left);
            char *lbl_else = new_label();
            char *lbl_end = new_label();
            tac_add("ifz", cond, NULL, lbl_else);
            gen_stmt(node->right);
            if (node->third) {
                tac_add("goto", NULL, NULL, lbl_end);
                tac_add("label", NULL, NULL, lbl_else);
                gen_stmt(node->third);
                tac_add("label", NULL, NULL, lbl_end);
            } else {
                tac_add("label", NULL, NULL, lbl_else);
            }
            free(cond);
            free(lbl_else);
            free(lbl_end);
            break;
        }
        case AST_WHILE: {
            char *lbl_start = new_label();
            char *lbl_end = new_label();
            tac_add("label", NULL, NULL, lbl_start);
            char *cond = gen_expr(node->left);
            tac_add("ifz", cond, NULL, lbl_end);
            gen_stmt(node->right);
            tac_add("goto", NULL, NULL, lbl_start);
            tac_add("label", NULL, NULL, lbl_end);
            free(cond);
            free(lbl_start);
            free(lbl_end);
            break;
        }
        default:
            break;
    }
}

void tac_reset(void) {
    TacInstr *cur = g_head;
    while (cur) {
        TacInstr *next = cur->next;
        free(cur->op);
        free(cur->arg1);
        free(cur->arg2);
        free(cur->result);
        free(cur);
        cur = next;
    }
    g_head = g_tail = NULL;
    g_temp_count = 0;
    g_label_count = 0;
}

void tac_generate(const AstNode *root) {
    gen_stmt(root);
}

void tac_print(FILE *out) {
    if (!out) return;
    const TacInstr *cur = g_head;
    while (cur) {
        if (strcmp(cur->op, "label") == 0) {
            fprintf(out, "%s:\n", cur->result);
        } else if (strcmp(cur->op, "goto") == 0) {
            fprintf(out, "goto %s\n", cur->result);
        } else if (strcmp(cur->op, "ifz") == 0) {
            fprintf(out, "ifz %s goto %s\n", cur->arg1, cur->result);
        } else if (strcmp(cur->op, "print") == 0) {
            fprintf(out, "print %s\n", cur->arg1);
        } else if (strcmp(cur->op, "return") == 0) {
            if (cur->arg1) {
                fprintf(out, "return %s\n", cur->arg1);
            } else {
                fprintf(out, "return\n");
            }
        } else if (strcmp(cur->op, "=") == 0) {
            fprintf(out, "%s = %s\n", cur->result, cur->arg1);
        } else {
            fprintf(out, "%s = %s %s %s\n", cur->result, cur->arg1, cur->op, cur->arg2);
        }
        cur = cur->next;
    }
}

const TacInstr *tac_get_head(void) {
    return g_head;
}
