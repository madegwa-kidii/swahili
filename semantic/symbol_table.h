#ifndef SWAHILI_SYMBOL_TABLE_H
#define SWAHILI_SYMBOL_TABLE_H

#include "../parser/ast.h"

typedef struct Symbol {
    char *name;
    ValueType type;
    int depth;
    struct Symbol *next;
} Symbol;

typedef struct {
    Symbol *head;
    int depth;
} SymbolTable;

void st_init(SymbolTable *st);
void st_enter_scope(SymbolTable *st);
void st_exit_scope(SymbolTable *st);
int st_declare(SymbolTable *st, const char *name, ValueType type);
Symbol *st_lookup(SymbolTable *st, const char *name);
void st_free(SymbolTable *st);

#endif
