#ifndef SWAHILI_PARSER_H
#define SWAHILI_PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"

ASTNode *parse_tokens(const TokenStream *stream);

#endif
