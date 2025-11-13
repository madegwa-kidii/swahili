#ifndef SWAHILI_PARSER_H
#define SWAHILI_PARSER_H

#include "lexer.h"

typedef enum {
    NODE_NUMBER,
    NODE_ADD,
    NODE_PRINT
} NodeType;

typedef struct Node {
    NodeType type;
    char *value;
    struct Node *left;
    struct Node *right;
} Node;

Node *parse(Token *tokens);
void free_ast(Node *node);

#endif
