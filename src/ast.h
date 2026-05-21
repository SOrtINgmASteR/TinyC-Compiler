#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_STMT_LIST,
    AST_DECL,
    AST_ASSIGN,
    AST_IF,
    AST_WHILE,
    AST_PRINT,
    AST_RETURN,
    AST_BINOP,
    AST_IDENT,
    AST_INT,
    AST_FLOAT,
    AST_STRING,
    AST_CHAR
} AstKind;

typedef struct AstNode {
    AstKind kind;
    char *text;
    struct AstNode *left;
    struct AstNode *right;
    struct AstNode *third;
} AstNode;

AstNode *ast_create(AstKind kind, const char *text, AstNode *left, AstNode *right, AstNode *third);
AstNode *ast_append(AstNode *list, AstNode *node);
void ast_print(const AstNode *node, int indent);
void ast_print_to_file(const AstNode *node, FILE *out, int indent);
void ast_free(AstNode *node);

#endif
