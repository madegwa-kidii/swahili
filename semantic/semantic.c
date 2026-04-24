#include "semantic.h"

#include <stdio.h>
#include <string.h>

static int errors = 0;

static const char *type_name(ValueType t) {
    switch (t) {
        case TYPE_INT: return "namba";
        case TYPE_STRING: return "maneno";
        default: return "void";
    }
}

static ValueType infer_expr(ASTNode *n, SymbolTable *st);

static void check_stmt(ASTNode *n, SymbolTable *st) {
    if (!n) return;

    switch (n->type) {
        case AST_VAR_DECL:
            if (!st_declare(st, n->name, n->value_type)) {
                fprintf(stderr, "Semantic error: variable '%s' imetangazwa mara mbili.\n", n->name);
                errors++;
            }
            if (n->right) {
                ValueType rhs = infer_expr(n->right, st);
                if (rhs != n->value_type) {
                    fprintf(stderr, "Semantic error: '%s' inahitaji %s lakini imepewa %s.\n",
                            n->name, type_name(n->value_type), type_name(rhs));
                    errors++;
                }
            }
            break;
        case AST_ASSIGN: {
            Symbol *s = st_lookup(st, n->name);
            if (!s) {
                fprintf(stderr, "Semantic error: variable '%s' halijatangazwa.\n", n->name);
                errors++;
                break;
            }
            ValueType rhs = infer_expr(n->right, st);
            if (rhs != s->type) {
                fprintf(stderr, "Semantic error: assignment ya '%s' inahitaji %s lakini imepewa %s.\n",
                        n->name, type_name(s->type), type_name(rhs));
                errors++;
            }
            break;
        }
        case AST_PRINT:
            (void)infer_expr(n->left, st);
            break;
        case AST_IF:
            if (infer_expr(n->condition, st) != TYPE_INT) {
                fprintf(stderr, "Semantic error: condition ya kama lazima iwe namba/bool.\n");
                errors++;
            }
            st_enter_scope(st);
            check_stmt(n->then_branch, st);
            st_exit_scope(st);
            if (n->else_branch) {
                st_enter_scope(st);
                check_stmt(n->else_branch, st);
                st_exit_scope(st);
            }
            break;
        case AST_WHILE:
            if (infer_expr(n->condition, st) != TYPE_INT) {
                fprintf(stderr, "Semantic error: condition ya wakati lazima iwe namba/bool.\n");
                errors++;
            }
            st_enter_scope(st);
            check_stmt(n->body, st);
            st_exit_scope(st);
            break;
        case AST_BLOCK:
        case AST_PROGRAM:
            for (int i = 0; i < n->child_count; i++) check_stmt(n->children[i], st);
            break;
        default:
            break;
    }
}

static ValueType infer_expr(ASTNode *n, SymbolTable *st) {
    if (!n) return TYPE_VOID;
    switch (n->type) {
        case AST_LITERAL_INT: return TYPE_INT;
        case AST_LITERAL_STRING: return TYPE_STRING;
        case AST_IDENTIFIER: {
            Symbol *s = st_lookup(st, n->name);
            if (!s) {
                fprintf(stderr, "Semantic error: variable '%s' limetumika bila declaration.\n", n->name);
                errors++;
                return TYPE_VOID;
            }
            return s->type;
        }
        case AST_BINOP: {
            ValueType l = infer_expr(n->left, st);
            ValueType r = infer_expr(n->right, st);
            if (strcmp(n->op, "+") == 0 || strcmp(n->op, "-") == 0 || strcmp(n->op, "*") == 0 || strcmp(n->op, "/") == 0) {
                if (l != TYPE_INT || r != TYPE_INT) {
                    fprintf(stderr, "Semantic error: operator '%s' unahitaji namba pekee.\n", n->op);
                    errors++;
                }
                return TYPE_INT;
            }
            if (strcmp(n->op, "==") == 0 || strcmp(n->op, "!=") == 0 || strcmp(n->op, "<") == 0 || strcmp(n->op, ">") == 0 || strcmp(n->op, "<=") == 0 || strcmp(n->op, ">=") == 0) {
                if (l != r) {
                    fprintf(stderr, "Semantic error: comparison inahitaji aina moja pande zote.\n");
                    errors++;
                }
                return TYPE_INT;
            }
            return TYPE_VOID;
        }
        default:
            return TYPE_VOID;
    }
}

int semantic_check(ASTNode *program, SymbolTable *table) {
    printf("\n=== 3) Semantic Analysis ===\n");
    errors = 0;
    st_init(table);
    check_stmt(program, table);
    if (errors == 0) {
        printf("Semantic check: PASS\n");
    } else {
        printf("Semantic check: FAIL (%d errors)\n", errors);
    }
    return errors == 0;
}
