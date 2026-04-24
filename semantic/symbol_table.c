#include "symbol_table.h"
#include "../utils.h"

#include <stdio.h>
#include <string.h>

void st_init(SymbolTable *st) {
    st->head = NULL;
    st->depth = 0;
}

void st_enter_scope(SymbolTable *st) { st->depth++; }

void st_exit_scope(SymbolTable *st) {
    Symbol **cur = &st->head;
    while (*cur) {
        if ((*cur)->depth == st->depth) {
            Symbol *dead = *cur;
            *cur = dead->next;
            free(dead->name);
            free(dead);
        } else {
            cur = &(*cur)->next;
        }
    }
    st->depth--;
}

int st_declare(SymbolTable *st, const char *name, ValueType type) {
    for (Symbol *s = st->head; s; s = s->next) {
        if (s->depth == st->depth && strcmp(s->name, name) == 0) return 0;
    }

    Symbol *s = (Symbol *)xmalloc(sizeof(Symbol));
    s->name = xstrdup(name);
    s->type = type;
    s->depth = st->depth;
    s->next = st->head;
    st->head = s;
    return 1;
}

Symbol *st_lookup(SymbolTable *st, const char *name) {
    for (Symbol *s = st->head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

void st_free(SymbolTable *st) {
    while (st->depth > 0) st_exit_scope(st);
    while (st->head) {
        Symbol *n = st->head->next;
        free(st->head->name);
        free(st->head);
        st->head = n;
    }
}
