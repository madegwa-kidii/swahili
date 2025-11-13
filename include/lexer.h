#ifndef SWAHILI_LEXER_H
#define SWAHILI_LEXER_H

typedef enum {
    TOKEN_PRINT,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SEMICOLON,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

Token *tokenize(const char *src, int *count);

#endif
