#include "symbol_table.h"

#include <stdlib.h>
#include <string.h>

static Symbol *g_head = NULL;
static Symbol *g_all_head = NULL;
static Symbol *g_all_tail = NULL;
static int g_scope = 0;

void symtab_init(void) {
    g_head = NULL;
    g_all_head = NULL;
    g_all_tail = NULL;
    g_scope = 0;
}

void symtab_enter_scope(void) {
    g_scope++;
}

void symtab_leave_scope(void) {
    Symbol *cur = g_head;
    Symbol *prev = NULL;

    while (cur) {
        if (cur->scope == g_scope) {
            if (prev) {
                prev->next = cur->next;
            } else {
                g_head = cur->next;
            }
            cur = cur->next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }

    if (g_scope > 0) {
        g_scope--;
    }
}

const Symbol *symtab_lookup(const char *name) {
    Symbol *cur = g_head;
    Symbol *best = NULL;
    int best_scope = -1;

    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (cur->scope >= best_scope) {
                best = cur;
                best_scope = cur->scope;
            }
        }
        cur = cur->next;
    }
    return best;
}

const Symbol *symtab_lookup_current(const char *name) {
    Symbol *cur = g_head;
    while (cur) {
        if (cur->scope == g_scope && strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

int symtab_insert(const char *name, const char *type, int line) {
    if (symtab_lookup_current(name)) {
        return -1;
    }

    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
    if (!sym) return -1;

    sym->name = strdup(name ? name : "");
    sym->type = strdup(type ? type : "unknown");
    sym->scope = g_scope;
    sym->line = line;
    sym->next = g_head;
    sym->next_all = NULL;
    g_head = sym;

    if (!g_all_head) {
        g_all_head = g_all_tail = sym;
    } else {
        g_all_tail->next_all = sym;
        g_all_tail = sym;
    }
    return 0;
}

const char *symtab_get_type(const char *name) {
    const Symbol *sym = symtab_lookup(name);
    return sym ? sym->type : NULL;
}

void symtab_print(FILE *out) {
    if (!out) return;
    fprintf(out, "%-15s %-10s %-6s %-5s\n", "Name", "Type", "Scope", "Line");
    fprintf(out, "------------------------------------------\n");

    const Symbol *cur = g_all_head;
    while (cur) {
        fprintf(out, "%-15s %-10s %-6d %-5d\n", cur->name, cur->type, cur->scope, cur->line);
        cur = cur->next_all;
    }
}
