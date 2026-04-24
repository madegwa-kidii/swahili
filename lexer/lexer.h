#ifndef SWAHILI_LEXER_H
#define SWAHILI_LEXER_H

typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,

    TOKEN_KW_NAMBA,
    TOKEN_KW_MANENO,
    TOKEN_KW_ANDIKA,
    TOKEN_KW_KAMA,
    TOKEN_KW_SIO,
    TOKEN_KW_WAKATI,
    TOKEN_KW_NI,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_ASSIGN,
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTE,
    TOKEN_GTE,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
    int line;
} Token;

typedef struct {
    Token *items;
    int count;
    int capacity;
} TokenStream;

TokenStream lex_source(const char *src);
void print_tokens(const TokenStream *ts);
void free_tokens(TokenStream *ts);
const char *token_type_name(TokenType t);

#endif
