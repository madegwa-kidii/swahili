#include "optimizer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_number(const char *s) {
    if (!s || !*s) return 0;
    int i = (s[0] == '-') ? 1 : 0;
    for (; s[i]; i++) if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

static int eval_bin(const char *op, int a, int b, int *ok) {
    *ok = 1;
    if (strcmp(op, "+") == 0) return a + b;
    if (strcmp(op, "-") == 0) return a - b;
    if (strcmp(op, "*") == 0) return a * b;
    if (strcmp(op, "/") == 0) { if (b == 0) { *ok = 0; return 0; } return a / b; }
    if (strcmp(op, "==") == 0) return a == b;
    if (strcmp(op, "!=") == 0) return a != b;
    if (strcmp(op, "<") == 0) return a < b;
    if (strcmp(op, ">") == 0) return a > b;
    if (strcmp(op, "<=") == 0) return a <= b;
    if (strcmp(op, ">=") == 0) return a >= b;
    *ok = 0;
    return 0;
}

IRList optimize_ir(const IRList *input) {
    IRList out = {0};
    out.capacity = input->count + 8;
    out.items = calloc((size_t)out.capacity, sizeof(TACInstr));

    int skip_until_label = 0;
    for (int i = 0; i < input->count; i++) {
        TACInstr in = input->items[i];

        if (skip_until_label && in.op != TAC_LABEL) continue;
        if (skip_until_label && in.op == TAC_LABEL) skip_until_label = 0;

        if (in.op == TAC_BINOP && is_number(in.arg1) && is_number(in.arg2)) {
            int ok = 0;
            int v = eval_bin(in.extra, atoi(in.arg1), atoi(in.arg2), &ok);
            if (ok) {
                in.op = TAC_ASSIGN;
                snprintf(in.arg1, sizeof(in.arg1), "%d", v);
                in.arg2[0] = '\0';
                in.extra[0] = '\0';
            }
        }

        if (in.op == TAC_IFZ_GOTO && is_number(in.arg1)) {
            if (atoi(in.arg1) == 0) {
                in.op = TAC_GOTO;
                in.arg1[0] = '\0';
            } else {
                continue;
            }
        }

        out.items[out.count++] = in;

        if (in.op == TAC_GOTO) skip_until_label = 1;
    }

    return out;
}
