#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

static void fprint_indent(FILE *out, int indent) {
    for (int i = 0; i < indent; i++) {
        fprintf(out, "  ");
    }
}

AstNode *ast_create(AstKind kind, const char *text, AstNode *left, AstNode *right, AstNode *third) {
    AstNode *node = (AstNode *)malloc(sizeof(AstNode));
    if (!node) return NULL;
    node->kind = kind;
    node->text = text ? strdup(text) : NULL;
    node->left = left;
    node->right = right;
    node->third = third;
    return node;
}

AstNode *ast_append(AstNode *list, AstNode *node) {
    if (!node) return list;
    if (!list) return ast_create(AST_STMT_LIST, NULL, node, NULL, NULL);

    AstNode *cur = list;
    while (cur->right) {
        cur = cur->right;
    }
    cur->right = ast_create(AST_STMT_LIST, NULL, node, NULL, NULL);
    return list;
}

static const char *kind_name(AstKind kind) {
    switch (kind) {
        case AST_PROGRAM: return "Program";
        case AST_FUNCTION: return "Function";
        case AST_STMT_LIST: return "StmtList";
        case AST_DECL: return "Declaration";
        case AST_ASSIGN: return "Assignment";
        case AST_IF: return "If";
        case AST_WHILE: return "While";
        case AST_PRINT: return "Print";
        case AST_RETURN: return "Return";
        case AST_BINOP: return "BinaryOp";
        case AST_IDENT: return "Identifier";
        case AST_INT: return "Int";
        case AST_FLOAT: return "Float";
        case AST_STRING: return "String";
        case AST_CHAR: return "Char";
        default: return "Node";
    }
}

void ast_print(const AstNode *node, int indent) {
    if (!node) return;

    if (node->kind == AST_STMT_LIST) {
        ast_print(node->left, indent);
        ast_print(node->right, indent);
        return;
    }

    print_indent(indent);
    if (node->text) {
        printf("%s: %s\n", kind_name(node->kind), node->text);
    } else {
        printf("%s\n", kind_name(node->kind));
    }

    ast_print(node->left, indent + 1);
    ast_print(node->right, indent + 1);
    ast_print(node->third, indent + 1);
}

void ast_print_to_file(const AstNode *node, FILE *out, int indent) {
    if (!node || !out) return;

    if (node->kind == AST_STMT_LIST) {
        ast_print_to_file(node->left, out, indent);
        ast_print_to_file(node->right, out, indent);
        return;
    }

    fprint_indent(out, indent);
    if (node->text) {
        fprintf(out, "%s: %s\n", kind_name(node->kind), node->text);
    } else {
        fprintf(out, "%s\n", kind_name(node->kind));
    }

    ast_print_to_file(node->left, out, indent + 1);
    ast_print_to_file(node->right, out, indent + 1);
    ast_print_to_file(node->third, out, indent + 1);
}

void ast_free(AstNode *node) {
    if (!node) return;
    ast_free(node->left);
    ast_free(node->right);
    ast_free(node->third);
    free(node->text);
    free(node);
}
