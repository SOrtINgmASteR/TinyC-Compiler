#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>

typedef struct Symbol {
    char *name;
    char *type;
    int scope;
    int line;
    struct Symbol *next;
    struct Symbol *next_all;
} Symbol;

void symtab_init(void);
void symtab_enter_scope(void);
void symtab_leave_scope(void);
int symtab_insert(const char *name, const char *type, int line);
const Symbol *symtab_lookup(const char *name);
const Symbol *symtab_lookup_current(const char *name);
const char *symtab_get_type(const char *name);
void symtab_print(FILE *out);

#endif
