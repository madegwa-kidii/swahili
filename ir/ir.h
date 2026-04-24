#ifndef SWAHILI_IR_H
#define SWAHILI_IR_H

#include "../parser/ast.h"

typedef enum {
    TAC_NOP,
    TAC_ASSIGN,
    TAC_BINOP,
    TAC_LABEL,
    TAC_GOTO,
    TAC_IFZ_GOTO,
    TAC_PRINT
} TACOp;

typedef struct {
    TACOp op;
    char result[32];
    char arg1[32];
    char arg2[32];
    char extra[16];
} TACInstr;

typedef struct {
    TACInstr *items;
    int count;
    int capacity;
} IRList;

IRList ir_generate(ASTNode *program);
void ir_print(const IRList *ir, const char *title);
void ir_free(IRList *ir);

#endif
