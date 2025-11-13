#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"

static int pos = 0;
static Token *toks;

static Token peek() { return toks[pos]; }
static Token advance() { return toks[pos++]; }

static Node *parse_expr();

static Node *parse_number() {
    Token t = advance();
    Node *n = malloc(sizeof(Node));
    n->type = NODE_NUMBER;
    n->value = strdup(t.value);
    n->left = n->right = NULL;
    return n;
}

static Node *parse_expr() {
    Node *left = parse_number();
    while (peek().type == TOKEN_PLUS) {
        advance(); // skip '+'
        Node *right = parse_number();
        Node *n = malloc(sizeof(Node));
        n->type = NODE_ADD;
        n->left = left;
        n->right = right;
        left = n;
    }
    return left;
}

Node *parse(Token *tokens_in) {
    toks = tokens_in;
    pos = 0;

    if (peek().type == TOKEN_PRINT) {
        advance();
        advance(); // skip '('
        Node *expr = parse_expr();
        advance(); // skip ')'
        advance(); // skip ';'
        Node *printNode = malloc(sizeof(Node));
        printNode->type = NODE_PRINT;
        printNode->left = expr;
        return printNode;
    }

    printf("Syntax error!\n");
    exit(1);
}
