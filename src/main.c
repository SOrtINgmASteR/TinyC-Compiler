#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ast.h"
#include "symbol_table.h"
#include "tac.h"
#include "codegen.h"

extern FILE *yyin;
extern int yyparse(void);
extern FILE *g_token_out;
extern AstNode *g_ast_root;
extern int g_semantic_errors;

static void ensure_output_dir(void) {
    struct stat st;
    if (stat("output", &st) != 0) {
        mkdir("output", 0777);
    }
}

static void dump_file(const char *title, const char *path) {
    printf("\n=== %s (%s) ===\n", title, path);
    FILE *in = fopen(path, "r");
    if (!in) {
        perror("Failed to read output file");
        return;
    }
    char buf[256];
    while (fgets(buf, sizeof(buf), in)) {
        fputs(buf, stdout);
    }
    fclose(in);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input-file>\n", argv[0]);
        return 1;
    }

    ensure_output_dir();

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Failed to open input file");
        return 1;
    }

    FILE *tok_out = fopen("output/tokens.txt", "w");
    FILE *sym_out = fopen("output/symbol_table.txt", "w");
    FILE *tac_out = fopen("output/tac.txt", "w");
    FILE *asm_out = fopen("output/assembly.asm", "w");
    FILE *parse_out = fopen("output/parser_output.txt", "w");

    if (!tok_out || !sym_out || !tac_out || !asm_out || !parse_out) {
        perror("Failed to open output files");
        fclose(in);
        if (tok_out) fclose(tok_out);
        if (sym_out) fclose(sym_out);
        if (tac_out) fclose(tac_out);
        if (asm_out) fclose(asm_out);
        if (parse_out) fclose(parse_out);
        return 1;
    }

    yyin = in;
    g_token_out = tok_out;

    symtab_init();

    int parse_result = yyparse();

    if (parse_result == 0) {
        printf("Parsing Successful\n");
        fprintf(parse_out, "Parsing Successful\n");
        printf("\n=== AST ===\n");
        fprintf(parse_out, "\n=== AST ===\n");
        ast_print(g_ast_root, 0);
        ast_print_to_file(g_ast_root, parse_out, 0);

        symtab_print(sym_out);

        tac_reset();
        tac_generate(g_ast_root);
        tac_print(tac_out);
        codegen_generate(tac_get_head(), asm_out);

        if (g_semantic_errors > 0) {
            printf("Semantic errors: %d\n", g_semantic_errors);
            fprintf(parse_out, "Semantic errors: %d\n", g_semantic_errors);
        }
    } else {
        fprintf(stderr, "Parsing failed due to syntax errors.\n");
        fprintf(parse_out, "Parsing failed due to syntax errors.\n");
    }

    fflush(tok_out);
    fflush(sym_out);
    fflush(tac_out);
    fflush(asm_out);
    fflush(parse_out);

    dump_file("TOKENS", "output/tokens.txt");
    dump_file("PARSER OUTPUT", "output/parser_output.txt");
    dump_file("SYMBOL TABLE", "output/symbol_table.txt");
    dump_file("THREE ADDRESS CODE", "output/tac.txt");
    dump_file("ASSEMBLY", "output/assembly.asm");

    fclose(asm_out);
    fclose(tac_out);
    fclose(sym_out);
    fclose(tok_out);
    fclose(parse_out);
    fclose(in);

    return 0;
}
