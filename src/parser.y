%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "symbol_table.h"

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s);

AstNode *g_ast_root = NULL;
const char *g_current_decl_type = NULL;
int g_semantic_errors = 0;

typedef enum {
    TYPE_UNKNOWN = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_VOID
} TypeKind;

static int type_from_name(const char *t) {
    if (!t) return TYPE_UNKNOWN;
    if (strcmp(t, "int") == 0) return TYPE_INT;
    if (strcmp(t, "float") == 0) return TYPE_FLOAT;
    if (strcmp(t, "char") == 0) return TYPE_CHAR;
    if (strcmp(t, "string") == 0) return TYPE_STRING;
    if (strcmp(t, "void") == 0) return TYPE_VOID;
    return TYPE_UNKNOWN;
}

static int is_numeric_type(int t) {
    return t == TYPE_INT || t == TYPE_FLOAT;
}

static void semantic_error(const char *msg, const char *name) {
    if (name) {
        fprintf(stderr, "Semantic Error (line %d): %s '%s'\n", yylineno, msg, name);
    } else {
        fprintf(stderr, "Semantic Error (line %d): %s\n", yylineno, msg);
    }
    g_semantic_errors++;
}

static char *dup_text(const char *s) {
    if (!s) return NULL;
    return strdup(s);
}

static const char *make_lit_string_int(int v) {
    static char buf[64];
    snprintf(buf, sizeof(buf), "%d", v);
    return buf;
}

static const char *make_lit_string_float(double v) {
    static char buf[64];
    snprintf(buf, sizeof(buf), "%g", v);
    return buf;
}

%}

%code requires {
    #include "ast.h"
    typedef struct {
        AstNode *node;
        int type;
    } ExprAttr;
}

%union {
    char *sval;
    int ival;
    double fval;
    AstNode *node;
    ExprAttr expr;
}

%token <sval> IDENT STRING_LIT CHAR_LIT
%token <ival> INT_LIT
%token <fval> FLOAT_LIT
%token IF ELSE WHILE RETURN PRINT
%token INT FLOAT CHAR VOID
%token EQ NE LE GE AND OR
%token KEYWORD UNKNOWN

%type <node> program function_list function compound stmt stmt_list stmt_list_opt decl assign if_stmt while_stmt print_stmt return_stmt print_arg ident_list
%type <sval> type
%type <expr> expr term factor cond expr_opt

%start program

%nonassoc IFX
%nonassoc ELSE
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right UMINUS

%%

program
    : function_list { g_ast_root = $1; }
    ;

function_list
    : function_list function { $$ = ast_append($1, $2); }
    | function { $$ = $1; }
    ;

function
    : type IDENT '(' ')' compound {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s %s", $1, $2);
        $$ = ast_create(AST_FUNCTION, buf, $5, NULL, NULL);
      }
    ;

compound
    : '{' { symtab_enter_scope(); } stmt_list_opt '}' {
        symtab_leave_scope();
        $$ = $3;
      }
    ;

stmt_list_opt
    : stmt_list { $$ = $1; }
    | { $$ = NULL; }
    ;

stmt_list
    : stmt_list stmt { $$ = ast_append($1, $2); }
    | stmt { $$ = ast_append(NULL, $1); }
    ;

stmt
    : decl ';' { $$ = $1; }
    | assign ';' { $$ = $1; }
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | print_stmt ';' { $$ = $1; }
    | return_stmt ';' { $$ = $1; }
    | compound { $$ = $1; }
    ;

decl
    : type ident_list { $$ = ast_create(AST_DECL, $1, $2, NULL, NULL); }
    ;

ident_list
    : ident_list ',' IDENT {
        if (symtab_insert($3, g_current_decl_type, yylineno) != 0) {
            semantic_error("Variable already declared", $3);
        }
        $$ = ast_append($1, ast_create(AST_IDENT, $3, NULL, NULL, NULL));
      }
    | IDENT {
        if (symtab_insert($1, g_current_decl_type, yylineno) != 0) {
            semantic_error("Variable already declared", $1);
        }
        $$ = ast_create(AST_IDENT, $1, NULL, NULL, NULL);
      }
    ;

type
    : INT { $$ = dup_text("int"); g_current_decl_type = $$; }
    | FLOAT { $$ = dup_text("float"); g_current_decl_type = $$; }
    | CHAR { $$ = dup_text("char"); g_current_decl_type = $$; }
    | VOID { $$ = dup_text("void"); g_current_decl_type = $$; }
    ;

assign
    : IDENT '=' expr {
        const char *lhs_type = symtab_get_type($1);
        if (!lhs_type) {
            semantic_error("Undeclared variable", $1);
        } else {
            int lt = type_from_name(lhs_type);
            if ($3.type != TYPE_UNKNOWN && lt != TYPE_UNKNOWN && lt != $3.type) {
                semantic_error("Type mismatch in assignment to", $1);
            }
        }
        $$ = ast_create(AST_ASSIGN, "=", ast_create(AST_IDENT, $1, NULL, NULL, NULL), $3.node, NULL);
      }
    ;

if_stmt
    : IF '(' cond ')' stmt %prec IFX { $$ = ast_create(AST_IF, "if", $3.node, $5, NULL); }
    | IF '(' cond ')' stmt ELSE stmt { $$ = ast_create(AST_IF, "if", $3.node, $5, $7); }
    ;

while_stmt
    : WHILE '(' cond ')' stmt { $$ = ast_create(AST_WHILE, "while", $3.node, $5, NULL); }
    ;

print_stmt
    : PRINT '(' print_arg ')' { $$ = ast_create(AST_PRINT, "print", $3, NULL, NULL); }
    ;

print_arg
    : expr { $$ = $1.node; }
    | STRING_LIT { $$ = ast_create(AST_STRING, $1, NULL, NULL, NULL); }
    ;

return_stmt
    : RETURN expr_opt { $$ = ast_create(AST_RETURN, "return", $2.node, NULL, NULL); }
    ;

expr_opt
    : expr { $$ = $1; }
    | { $$.node = NULL; $$.type = TYPE_VOID; }
    ;

cond
    : expr { $$ = $1; }
    | expr EQ expr {
        $$.node = ast_create(AST_BINOP, "==", $1.node, $3.node, NULL);
        $$.type = TYPE_INT;
      }
    | expr NE expr {
        $$.node = ast_create(AST_BINOP, "!=", $1.node, $3.node, NULL);
        $$.type = TYPE_INT;
      }
    | expr LE expr {
        $$.node = ast_create(AST_BINOP, "<=", $1.node, $3.node, NULL);
        $$.type = TYPE_INT;
      }
    | expr GE expr {
        $$.node = ast_create(AST_BINOP, ">=", $1.node, $3.node, NULL);
        $$.type = TYPE_INT;
      }
    | expr '<' expr {
        $$.node = ast_create(AST_BINOP, "<", $1.node, $3.node, NULL);
        $$.type = TYPE_INT;
      }
    | expr '>' expr {
        $$.node = ast_create(AST_BINOP, ">", $1.node, $3.node, NULL);
        $$.type = TYPE_INT;
      }
    ;

expr
    : expr '+' term {
        if (!is_numeric_type($1.type) || !is_numeric_type($3.type)) {
            semantic_error("Non-numeric types in expression", NULL);
            $$.type = TYPE_UNKNOWN;
        } else {
            $$.type = ($1.type == TYPE_FLOAT || $3.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
        }
        $$.node = ast_create(AST_BINOP, "+", $1.node, $3.node, NULL);
      }
    | expr '-' term {
        if (!is_numeric_type($1.type) || !is_numeric_type($3.type)) {
            semantic_error("Non-numeric types in expression", NULL);
            $$.type = TYPE_UNKNOWN;
        } else {
            $$.type = ($1.type == TYPE_FLOAT || $3.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
        }
        $$.node = ast_create(AST_BINOP, "-", $1.node, $3.node, NULL);
      }
    | term { $$ = $1; }
    ;

term
    : term '*' factor {
        if (!is_numeric_type($1.type) || !is_numeric_type($3.type)) {
            semantic_error("Non-numeric types in expression", NULL);
            $$.type = TYPE_UNKNOWN;
        } else {
            $$.type = ($1.type == TYPE_FLOAT || $3.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
        }
        $$.node = ast_create(AST_BINOP, "*", $1.node, $3.node, NULL);
      }
    | term '/' factor {
        if (!is_numeric_type($1.type) || !is_numeric_type($3.type)) {
            semantic_error("Non-numeric types in expression", NULL);
            $$.type = TYPE_UNKNOWN;
        } else {
            $$.type = ($1.type == TYPE_FLOAT || $3.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
        }
        $$.node = ast_create(AST_BINOP, "/", $1.node, $3.node, NULL);
      }
    | term '%' factor {
        if ($1.type != TYPE_INT || $3.type != TYPE_INT) {
            semantic_error("Modulo requires integer types", NULL);
            $$.type = TYPE_UNKNOWN;
        } else {
            $$.type = TYPE_INT;
        }
        $$.node = ast_create(AST_BINOP, "%", $1.node, $3.node, NULL);
      }
    | factor { $$ = $1; }
    ;

factor
    : '(' expr ')' { $$ = $2; }
    | IDENT {
        const char *t = symtab_get_type($1);
        if (!t) {
            semantic_error("Undeclared variable", $1);
            $$.type = TYPE_UNKNOWN;
        } else {
            $$.type = type_from_name(t);
        }
        $$.node = ast_create(AST_IDENT, $1, NULL, NULL, NULL);
      }
    | INT_LIT {
        $$.type = TYPE_INT;
        $$.node = ast_create(AST_INT, make_lit_string_int($1), NULL, NULL, NULL);
      }
    | FLOAT_LIT {
        $$.type = TYPE_FLOAT;
        $$.node = ast_create(AST_FLOAT, make_lit_string_float($1), NULL, NULL, NULL);
      }
    | CHAR_LIT {
        $$.type = TYPE_CHAR;
        $$.node = ast_create(AST_CHAR, $1, NULL, NULL, NULL);
      }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error (line %d): %s\n", yylineno, s);
}
