#include "lexer.h"
#include "../utils.h"

#include <ctype.h>
#include <stdio.h>

static void push_token(TokenStream *ts, Token tok) {
    if (ts->count == ts->capacity) {
        ts->capacity = ts->capacity == 0 ? 64 : ts->capacity * 2;
        ts->items = realloc(ts->items, (size_t)ts->capacity * sizeof(Token));
        if (!ts->items) {
            fprintf(stderr, "Kosa: realloc imeshindikana.\n");
            exit(1);
        }
    }
    ts->items[ts->count++] = tok;
}

static TokenType keyword_or_ident(const char *s) {
    if (strcmp(s, "namba") == 0) return TOKEN_KW_NAMBA;
    if (strcmp(s, "maneno") == 0) return TOKEN_KW_MANENO;
    if (strcmp(s, "andika") == 0) return TOKEN_KW_ANDIKA;
    if (strcmp(s, "kama") == 0) return TOKEN_KW_KAMA;
    if (strcmp(s, "sio") == 0) return TOKEN_KW_SIO;
    if (strcmp(s, "wakati") == 0) return TOKEN_KW_WAKATI;
    if (strcmp(s, "ni") == 0) return TOKEN_KW_NI;
    return TOKEN_IDENTIFIER;
}

TokenStream lex_source(const char *src) {
    TokenStream ts = {0};
    int i = 0;
    int line = 1;

    while (src[i]) {
        if (src[i] == '\n') {
            line++;
            i++;
            continue;
        }
        if (isspace((unsigned char)src[i])) {
            i++;
            continue;
        }
        if (src[i] == '#') {
            while (src[i] && src[i] != '\n') i++;
            continue;
        }

        if (isalpha((unsigned char)src[i]) || src[i] == '_') {
            int start = i;
            while (isalnum((unsigned char)src[i]) || src[i] == '_') i++;
            int len = i - start;
            char *lex = (char *)xmalloc((size_t)len + 1);
            memcpy(lex, src + start, (size_t)len);
            lex[len] = '\0';
            push_token(&ts, (Token){keyword_or_ident(lex), lex, line});
            continue;
        }

        if (isdigit((unsigned char)src[i])) {
            int start = i;
            while (isdigit((unsigned char)src[i])) i++;
            int len = i - start;
            char *lex = (char *)xmalloc((size_t)len + 1);
            memcpy(lex, src + start, (size_t)len);
            lex[len] = '\0';
            push_token(&ts, (Token){TOKEN_NUMBER, lex, line});
            continue;
        }

        if (src[i] == '"') {
            i++;
            int start = i;
            while (src[i] && src[i] != '"') {
                if (src[i] == '\n') line++;
                i++;
            }
            if (!src[i]) {
                fprintf(stderr, "Lexer error line %d: string haijafungwa.\n", line);
                exit(1);
            }
            int len = i - start;
            char *lex = (char *)xmalloc((size_t)len + 1);
            memcpy(lex, src + start, (size_t)len);
            lex[len] = '\0';
            i++;
            push_token(&ts, (Token){TOKEN_STRING, lex, line});
            continue;
        }

        if (src[i] == '=' && src[i + 1] == '=') { push_token(&ts, (Token){TOKEN_EQ, xstrdup("=="), line}); i += 2; continue; }
        if (src[i] == '!' && src[i + 1] == '=') { push_token(&ts, (Token){TOKEN_NEQ, xstrdup("!="), line}); i += 2; continue; }
        if (src[i] == '<' && src[i + 1] == '=') { push_token(&ts, (Token){TOKEN_LTE, xstrdup("<="), line}); i += 2; continue; }
        if (src[i] == '>' && src[i + 1] == '=') { push_token(&ts, (Token){TOKEN_GTE, xstrdup(">="), line}); i += 2; continue; }

        switch (src[i]) {
            case '+': push_token(&ts, (Token){TOKEN_PLUS, xstrdup("+"), line}); break;
            case '-': push_token(&ts, (Token){TOKEN_MINUS, xstrdup("-"), line}); break;
            case '*': push_token(&ts, (Token){TOKEN_STAR, xstrdup("*"), line}); break;
            case '/': push_token(&ts, (Token){TOKEN_SLASH, xstrdup("/"), line}); break;
            case '=': push_token(&ts, (Token){TOKEN_ASSIGN, xstrdup("="), line}); break;
            case '<': push_token(&ts, (Token){TOKEN_LT, xstrdup("<"), line}); break;
            case '>': push_token(&ts, (Token){TOKEN_GT, xstrdup(">"), line}); break;
            case '(': push_token(&ts, (Token){TOKEN_LPAREN, xstrdup("("), line}); break;
            case ')': push_token(&ts, (Token){TOKEN_RPAREN, xstrdup(")"), line}); break;
            case '{': push_token(&ts, (Token){TOKEN_LBRACE, xstrdup("{"), line}); break;
            case '}': push_token(&ts, (Token){TOKEN_RBRACE, xstrdup("}"), line}); break;
            case ';': push_token(&ts, (Token){TOKEN_SEMICOLON, xstrdup(";"), line}); break;
            case ',': push_token(&ts, (Token){TOKEN_COMMA, xstrdup(","), line}); break;
            default:
                fprintf(stderr, "Lexer error line %d: herufi isiyojulikana '%c'.\n", line, src[i]);
                exit(1);
        }
        i++;
    }

    push_token(&ts, (Token){TOKEN_EOF, xstrdup("<EOF>"), line});
    return ts;
}

const char *token_type_name(TokenType t) {
    switch (t) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_KW_NAMBA: return "KW_NAMBA";
        case TOKEN_KW_MANENO: return "KW_MANENO";
        case TOKEN_KW_ANDIKA: return "KW_ANDIKA";
        case TOKEN_KW_KAMA: return "KW_KAMA";
        case TOKEN_KW_SIO: return "KW_SIO";
        case TOKEN_KW_WAKATI: return "KW_WAKATI";
        case TOKEN_KW_NI: return "KW_NI";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NEQ: return "NEQ";
        case TOKEN_LT: return "LT";
        case TOKEN_GT: return "GT";
        case TOKEN_LTE: return "LTE";
        case TOKEN_GTE: return "GTE";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        default: return "UNKNOWN";
    }
}

void print_tokens(const TokenStream *ts) {
    printf("=== 1) Lexical Analysis (Tokens) ===\n");
    for (int i = 0; i < ts->count; i++) {
        printf("[%03d] line=%d %-12s '%s'\n", i, ts->items[i].line,
               token_type_name(ts->items[i].type), ts->items[i].lexeme);
    }
}

void free_tokens(TokenStream *ts) {
    for (int i = 0; i < ts->count; i++) free(ts->items[i].lexeme);
    free(ts->items);
    ts->items = NULL;
    ts->count = ts->capacity = 0;
}
