#include "codegen.h"

#include <stdio.h>
#include <string.h>

void generate_pseudo_assembly(const IRList *ir) {
    printf("\n=== 6) Code Generation (Pseudo Assembly) ===\n");
    for (int i = 0; i < ir->count; i++) {
        const TACInstr *in = &ir->items[i];
        switch (in->op) {
            case TAC_ASSIGN:
                printf("MOV %s, %s\n", in->result, in->arg1);
                break;
            case TAC_BINOP:
                printf("MOV R0, %s\n", in->arg1);
                if (strcmp(in->extra, "+") == 0) printf("ADD R0, %s\n", in->arg2);
                else if (strcmp(in->extra, "-") == 0) printf("SUB R0, %s\n", in->arg2);
                else if (strcmp(in->extra, "*") == 0) printf("MUL R0, %s\n", in->arg2);
                else if (strcmp(in->extra, "/") == 0) printf("DIV R0, %s\n", in->arg2);
                else printf("CMP R0, %s   ; op %s\n", in->arg2, in->extra);
                printf("MOV %s, R0\n", in->result);
                break;
            case TAC_LABEL:
                printf("%s:\n", in->result);
                break;
            case TAC_GOTO:
                printf("JMP %s\n", in->result);
                break;
            case TAC_IFZ_GOTO:
                printf("CMP %s, 0\n", in->arg1);
                printf("JE %s\n", in->result);
                break;
            case TAC_PRINT:
                printf("PRINT %s\n", in->arg1);
                break;
            default:
                break;
        }
    }
}
