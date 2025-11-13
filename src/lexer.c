#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/lexer.h"

Token *tokenize(const char *src, int *count) {
    Token *tokens = malloc(sizeof(Token) * 256);
    int i = 0, pos = 0;

    while (src[pos] != '\0') {
        if (isspace(src[pos])) { pos++; continue; }

        if (isdigit(src[pos])) {
            char buf[64]; int j = 0;
            while (isdigit(src[pos])) buf[j++] = src[pos++];
            buf[j] = '\0';
            tokens[i++] = (Token){TOKEN_NUMBER, strdup(buf)};
            continue;
        }

        if (strncmp(&src[pos], "chapisha", 8) == 0) {
            tokens[i++] = (Token){TOKEN_PRINT, strdup("chapisha")};
            pos += 8; continue;
        }

        switch (src[pos]) {
            case '+': tokens[i++] = (Token){TOKEN_PLUS, strdup("+")}; break;
            case '(': tokens[i++] = (Token){TOKEN_LPAREN, strdup("(")}; break;
            case ')': tokens[i++] = (Token){TOKEN_RPAREN, strdup(")")}; break;
            case ';': tokens[i++] = (Token){TOKEN_SEMICOLON, strdup(";")}; break;
            default:
                printf("Unknown character: %c\n", src[pos]);
                exit(1);
        }
        pos++;
    }

    tokens[i++] = (Token){TOKEN_EOF, strdup("EOF")};
    *count = i;
    return tokens;
}
