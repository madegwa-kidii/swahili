#include "ir.h"
#include "../utils.h"

#include <stdio.h>
#include <string.h>

static void emit(IRList *ir, TACInstr in) {
    if (ir->count == ir->capacity) {
        ir->capacity = ir->capacity ? ir->capacity * 2 : 64;
        ir->items = realloc(ir->items, (size_t)ir->capacity * sizeof(TACInstr));
        if (!ir->items) {
            fprintf(stderr, "Kosa: realloc imeshindikana.\n");
            exit(1);
        }
    }
    ir->items[ir->count++] = in;
}

static int temp_counter = 0;
static int label_counter = 0;

static void new_temp(char out[32]) { snprintf(out, 32, "t%d", temp_counter++); }
static void new_label(char out[32]) { snprintf(out, 32, "L%d", label_counter++); }

static void gen_stmt(IRList *ir, ASTNode *n);
static void gen_block(IRList *ir, ASTNode *b) {
    for (int i = 0; i < b->child_count; i++) gen_stmt(ir, b->children[i]);
}

static void gen_expr(IRList *ir, ASTNode *e, char out[32]) {
    if (e->type == AST_LITERAL_INT) {
        snprintf(out, 32, "%d", e->int_value);
        return;
    }
    if (e->type == AST_IDENTIFIER) {
        snprintf(out, 32, "%s", e->name);
        return;
    }
    if (e->type == AST_LITERAL_STRING) {
        snprintf(out, 32, "\"%s\"", e->str_value);
        return;
    }
    if (e->type == AST_BINOP) {
        char l[32], r[32], t[32];
        gen_expr(ir, e->left, l);
        gen_expr(ir, e->right, r);
        new_temp(t);
        emit(ir, (TACInstr){.op = TAC_BINOP});
        strcpy(ir->items[ir->count - 1].result, t);
        strcpy(ir->items[ir->count - 1].arg1, l);
        strcpy(ir->items[ir->count - 1].arg2, r);
        strcpy(ir->items[ir->count - 1].extra, e->op);
        strcpy(out, t);
        return;
    }
    strcpy(out, "0");
}

static void gen_stmt(IRList *ir, ASTNode *n) {
    if (!n) return;
    if (n->type == AST_VAR_DECL) {
        if (n->right) {
            char rhs[32];
            gen_expr(ir, n->right, rhs);
            emit(ir, (TACInstr){.op = TAC_ASSIGN});
            strcpy(ir->items[ir->count - 1].result, n->name);
            strcpy(ir->items[ir->count - 1].arg1, rhs);
        }
    } else if (n->type == AST_ASSIGN) {
        char rhs[32];
        gen_expr(ir, n->right, rhs);
        emit(ir, (TACInstr){.op = TAC_ASSIGN});
        strcpy(ir->items[ir->count - 1].result, n->name);
        strcpy(ir->items[ir->count - 1].arg1, rhs);
    } else if (n->type == AST_PRINT) {
        char v[32];
        gen_expr(ir, n->left, v);
        emit(ir, (TACInstr){.op = TAC_PRINT});
        strcpy(ir->items[ir->count - 1].arg1, v);
    } else if (n->type == AST_IF) {
        char cond[32], l_else[32], l_end[32];
        gen_expr(ir, n->condition, cond);
        new_label(l_else);
        new_label(l_end);
        emit(ir, (TACInstr){.op = TAC_IFZ_GOTO});
        strcpy(ir->items[ir->count - 1].arg1, cond);
        strcpy(ir->items[ir->count - 1].result, l_else);
        gen_stmt(ir, n->then_branch);
        emit(ir, (TACInstr){.op = TAC_GOTO});
        strcpy(ir->items[ir->count - 1].result, l_end);
        emit(ir, (TACInstr){.op = TAC_LABEL});
        strcpy(ir->items[ir->count - 1].result, l_else);
        if (n->else_branch) gen_stmt(ir, n->else_branch);
        emit(ir, (TACInstr){.op = TAC_LABEL});
        strcpy(ir->items[ir->count - 1].result, l_end);
    } else if (n->type == AST_WHILE) {
        char l_start[32], l_end[32], cond[32];
        new_label(l_start);
        new_label(l_end);
        emit(ir, (TACInstr){.op = TAC_LABEL});
        strcpy(ir->items[ir->count - 1].result, l_start);
        gen_expr(ir, n->condition, cond);
        emit(ir, (TACInstr){.op = TAC_IFZ_GOTO});
        strcpy(ir->items[ir->count - 1].arg1, cond);
        strcpy(ir->items[ir->count - 1].result, l_end);
        gen_stmt(ir, n->body);
        emit(ir, (TACInstr){.op = TAC_GOTO});
        strcpy(ir->items[ir->count - 1].result, l_start);
        emit(ir, (TACInstr){.op = TAC_LABEL});
        strcpy(ir->items[ir->count - 1].result, l_end);
    } else if (n->type == AST_PROGRAM || n->type == AST_BLOCK) {
        gen_block(ir, n);
    }
}

IRList ir_generate(ASTNode *program) {
    IRList ir = {0};
    temp_counter = 0;
    label_counter = 0;
    gen_stmt(&ir, program);
    return ir;
}

void ir_print(const IRList *ir, const char *title) {
    printf("\n=== %s ===\n", title);
    for (int i = 0; i < ir->count; i++) {
        const TACInstr *in = &ir->items[i];
        printf("[%03d] ", i);
        switch (in->op) {
            case TAC_ASSIGN: printf("%s = %s", in->result, in->arg1); break;
            case TAC_BINOP: printf("%s = %s %s %s", in->result, in->arg1, in->extra, in->arg2); break;
            case TAC_LABEL: printf("%s:", in->result); break;
            case TAC_GOTO: printf("goto %s", in->result); break;
            case TAC_IFZ_GOTO: printf("ifz %s goto %s", in->arg1, in->result); break;
            case TAC_PRINT: printf("print %s", in->arg1); break;
            default: printf("nop");
        }
        printf("\n");
    }
}

void ir_free(IRList *ir) {
    free(ir->items);
    ir->items = NULL;
    ir->count = ir->capacity = 0;
}
