#ifndef SWAHILI_PARSER_H
#define SWAHILI_PARSER_H

#include "lexer.h"

typedef enum {
    NODE_NUMBER,
    NODE_STRING,
    NODE_ADD,
    NODE_PRINT,
    NODE_PROGRAM
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
