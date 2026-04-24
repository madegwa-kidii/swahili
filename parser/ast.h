#ifndef SWAHILI_AST_H
#define SWAHILI_AST_H

typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_VOID
} ValueType;

typedef enum {
    AST_PROGRAM,
    AST_BLOCK,
    AST_VAR_DECL,
    AST_ASSIGN,
    AST_PRINT,
    AST_IF,
    AST_WHILE,
    AST_BINOP,
    AST_LITERAL_INT,
    AST_LITERAL_STRING,
    AST_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;
    ValueType value_type;
    char *name;
    char *op;
    int int_value;
    char *str_value;

    ASTNode **children;
    int child_count;
    int child_capacity;

    ASTNode *left;
    ASTNode *right;
    ASTNode *condition;
    ASTNode *then_branch;
    ASTNode *else_branch;
    ASTNode *body;
};

ASTNode *ast_new(ASTNodeType type);
void ast_add_child(ASTNode *parent, ASTNode *child);
void print_ast(const ASTNode *node, int indent);
void free_ast(ASTNode *node);

#endif
