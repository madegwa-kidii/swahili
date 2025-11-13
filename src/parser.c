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

static Node *parse_term() {
    Token t = peek();
    if (t.type == TOKEN_NUMBER) return parse_number();
    if (t.type == TOKEN_STRING) {
        advance();
        Node *n = malloc(sizeof(Node));
        n->type = NODE_STRING;
        n->value = strdup(t.value);
        n->left = n->right = NULL;
        return n;
    }

    printf("Syntax error: unexpected token %s\n", t.value);
    exit(1);
}


static Node *parse_expr() {
    Node *left = parse_term();
    while (peek().type == TOKEN_PLUS) {
        advance(); // skip '+'
        Node *right = parse_term();
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

    Node *program = malloc(sizeof(Node));
    program->type = NODE_PROGRAM;
    program->left = program->right = NULL;

    Node *last = NULL;

    while (peek().type != TOKEN_EOF) {
        if (peek().type != TOKEN_PRINT) {
            printf("Syntax error: expected 'andika'\n");
            exit(1);
        }

        advance();         // skip 'andika'
        advance();         // skip '('
        Node *expr = parse_expr();
        advance();         // skip ')'
        advance();         // skip ';'

        Node *printNode = malloc(sizeof(Node));
        printNode->type = NODE_PRINT;
        printNode->left = expr;
        printNode->left = expr;
        printNode->right = NULL;

        if (!program->left) {
            program->left = printNode;
        } else {
            last->right = printNode;
        }
        last = printNode;
    }

    return program;
}

