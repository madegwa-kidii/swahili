#include "parser.h"
#include "../utils.h"

#include <stdio.h>

typedef struct {
    const TokenStream *stream;
    int pos;
} Parser;

ASTNode *ast_new(ASTNodeType type) {
    ASTNode *n = (ASTNode *)xmalloc(sizeof(ASTNode));
    *n = (ASTNode){0};
    n->type = type;
    n->value_type = TYPE_VOID;
    return n;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (parent->child_count == parent->child_capacity) {
        parent->child_capacity = parent->child_capacity ? parent->child_capacity * 2 : 4;
        parent->children = realloc(parent->children, (size_t)parent->child_capacity * sizeof(ASTNode *));
        if (!parent->children) {
            fprintf(stderr, "Kosa: realloc imeshindikana.\n");
            exit(1);
        }
    }
    parent->children[parent->child_count++] = child;
}

static const Token *peek(Parser *p) { return &p->stream->items[p->pos]; }
static const Token *prev(Parser *p) { return &p->stream->items[p->pos - 1]; }
static int is_at_end(Parser *p) { return peek(p)->type == TOKEN_EOF; }
static const Token *advance(Parser *p) { if (!is_at_end(p)) p->pos++; return prev(p); }
static int check(Parser *p, TokenType t) { return peek(p)->type == t; }

static void expect(Parser *p, TokenType t, const char *msg) {
    if (check(p, t)) {
        advance(p);
        return;
    }
    fprintf(stderr, "Parser error line %d: %s, imepatikana %s\n",
            peek(p)->line, msg, token_type_name(peek(p)->type));
    exit(1);
}

static int match(Parser *p, TokenType t) {
    if (check(p, t)) {
        advance(p);
        return 1;
    }
    return 0;
}

static ASTNode *parse_expression(Parser *p);

static ASTNode *parse_primary(Parser *p) {
    if (match(p, TOKEN_NUMBER)) {
        ASTNode *n = ast_new(AST_LITERAL_INT);
        n->int_value = atoi(prev(p)->lexeme);
        n->value_type = TYPE_INT;
        return n;
    }
    if (match(p, TOKEN_STRING)) {
        ASTNode *n = ast_new(AST_LITERAL_STRING);
        n->str_value = xstrdup(prev(p)->lexeme);
        n->value_type = TYPE_STRING;
        return n;
    }
    if (match(p, TOKEN_IDENTIFIER)) {
        ASTNode *n = ast_new(AST_IDENTIFIER);
        n->name = xstrdup(prev(p)->lexeme);
        return n;
    }
    if (match(p, TOKEN_LPAREN)) {
        ASTNode *e = parse_expression(p);
        expect(p, TOKEN_RPAREN, "Unatarajia ')' ");
        return e;
    }

    fprintf(stderr, "Parser error line %d: expression haijakamilika\n", peek(p)->line);
    exit(1);
}

static ASTNode *parse_factor(Parser *p) {
    ASTNode *expr = parse_primary(p);
    while (check(p, TOKEN_STAR) || check(p, TOKEN_SLASH)) {
        const Token *op = advance(p);
        ASTNode *right = parse_primary(p);
        ASTNode *bin = ast_new(AST_BINOP);
        bin->op = xstrdup(op->lexeme);
        bin->left = expr;
        bin->right = right;
        expr = bin;
    }
    return expr;
}

static ASTNode *parse_term(Parser *p) {
    ASTNode *expr = parse_factor(p);
    while (check(p, TOKEN_PLUS) || check(p, TOKEN_MINUS)) {
        const Token *op = advance(p);
        ASTNode *right = parse_factor(p);
        ASTNode *bin = ast_new(AST_BINOP);
        bin->op = xstrdup(op->lexeme);
        bin->left = expr;
        bin->right = right;
        expr = bin;
    }
    return expr;
}

static ASTNode *parse_comparison(Parser *p) {
    ASTNode *expr = parse_term(p);
    while (check(p, TOKEN_LT) || check(p, TOKEN_GT) || check(p, TOKEN_LTE) || check(p, TOKEN_GTE)) {
        const Token *op = advance(p);
        ASTNode *right = parse_term(p);
        ASTNode *bin = ast_new(AST_BINOP);
        bin->op = xstrdup(op->lexeme);
        bin->left = expr;
        bin->right = right;
        expr = bin;
    }
    return expr;
}

static ASTNode *parse_equality(Parser *p) {
    ASTNode *expr = parse_comparison(p);
    while (check(p, TOKEN_EQ) || check(p, TOKEN_NEQ)) {
        const Token *op = advance(p);
        ASTNode *right = parse_comparison(p);
        ASTNode *bin = ast_new(AST_BINOP);
        bin->op = xstrdup(op->lexeme);
        bin->left = expr;
        bin->right = right;
        expr = bin;
    }
    return expr;
}

static ASTNode *parse_expression(Parser *p) { return parse_equality(p); }

static ASTNode *parse_block(Parser *p);

static ASTNode *parse_statement(Parser *p) {
    if (check(p, TOKEN_KW_NAMBA) || check(p, TOKEN_KW_MANENO)) {
        TokenType t = advance(p)->type;
        expect(p, TOKEN_IDENTIFIER, "Unatarajia jina la variable");
        ASTNode *decl = ast_new(AST_VAR_DECL);
        decl->name = xstrdup(prev(p)->lexeme);
        decl->value_type = (t == TOKEN_KW_NAMBA) ? TYPE_INT : TYPE_STRING;

        if (match(p, TOKEN_ASSIGN) || match(p, TOKEN_KW_NI)) {
            decl->right = parse_expression(p);
        }
        expect(p, TOKEN_SEMICOLON, "Unatarajia ';' baada ya declaration");
        return decl;
    }

    if (match(p, TOKEN_KW_ANDIKA)) {
        expect(p, TOKEN_LPAREN, "Unatarajia '('");
        ASTNode *st = ast_new(AST_PRINT);
        st->left = parse_expression(p);
        expect(p, TOKEN_RPAREN, "Unatarajia ')'");
        expect(p, TOKEN_SEMICOLON, "Unatarajia ';'");
        return st;
    }

    if (match(p, TOKEN_KW_KAMA)) {
        ASTNode *st = ast_new(AST_IF);
        expect(p, TOKEN_LPAREN, "Unatarajia '(' baada ya kama");
        st->condition = parse_expression(p);
        expect(p, TOKEN_RPAREN, "Unatarajia ')' ");
        st->then_branch = parse_block(p);
        if (match(p, TOKEN_KW_SIO)) {
            st->else_branch = parse_block(p);
        }
        return st;
    }

    if (match(p, TOKEN_KW_WAKATI)) {
        ASTNode *st = ast_new(AST_WHILE);
        expect(p, TOKEN_LPAREN, "Unatarajia '('");
        st->condition = parse_expression(p);
        expect(p, TOKEN_RPAREN, "Unatarajia ')' ");
        st->body = parse_block(p);
        return st;
    }

    if (check(p, TOKEN_IDENTIFIER) && p->stream->items[p->pos + 1].type == TOKEN_ASSIGN) {
        ASTNode *st = ast_new(AST_ASSIGN);
        st->name = xstrdup(advance(p)->lexeme);
        expect(p, TOKEN_ASSIGN, "Unatarajia '='");
        st->right = parse_expression(p);
        expect(p, TOKEN_SEMICOLON, "Unatarajia ';'");
        return st;
    }

    fprintf(stderr, "Parser error line %d: statement haijulikani (%s).\n", peek(p)->line, peek(p)->lexeme);
    exit(1);
}

static ASTNode *parse_block(Parser *p) {
    expect(p, TOKEN_LBRACE, "Unatarajia '{'");
    ASTNode *block = ast_new(AST_BLOCK);
    while (!check(p, TOKEN_RBRACE) && !is_at_end(p)) {
        ast_add_child(block, parse_statement(p));
    }
    expect(p, TOKEN_RBRACE, "Unatarajia '}'");
    return block;
}

ASTNode *parse_tokens(const TokenStream *stream) {
    Parser p = {.stream = stream, .pos = 0};
    ASTNode *program = ast_new(AST_PROGRAM);
    while (!is_at_end(&p)) {
        ast_add_child(program, parse_statement(&p));
    }
    return program;
}

static void print_indent(int indent) { for (int i = 0; i < indent; i++) printf("  "); }

void print_ast(const ASTNode *node, int indent) {
    if (!node) return;
    print_indent(indent);

    switch (node->type) {
        case AST_PROGRAM: printf("Program\n"); break;
        case AST_BLOCK: printf("Block\n"); break;
        case AST_VAR_DECL: printf("VarDecl %s (%s)\n", node->name, node->value_type == TYPE_INT ? "namba" : "maneno"); break;
        case AST_ASSIGN: printf("Assign %s\n", node->name); break;
        case AST_PRINT: printf("Print\n"); break;
        case AST_IF: printf("If\n"); break;
        case AST_WHILE: printf("While\n"); break;
        case AST_BINOP: printf("BinOp '%s'\n", node->op); break;
        case AST_LITERAL_INT: printf("Int %d\n", node->int_value); break;
        case AST_LITERAL_STRING: printf("String \"%s\"\n", node->str_value); break;
        case AST_IDENTIFIER: printf("Identifier %s\n", node->name); break;
    }

    if (node->condition) { print_indent(indent + 1); printf("condition:\n"); print_ast(node->condition, indent + 2); }
    if (node->then_branch) { print_indent(indent + 1); printf("then:\n"); print_ast(node->then_branch, indent + 2); }
    if (node->else_branch) { print_indent(indent + 1); printf("else:\n"); print_ast(node->else_branch, indent + 2); }
    if (node->body) { print_indent(indent + 1); printf("body:\n"); print_ast(node->body, indent + 2); }
    if (node->left) { print_indent(indent + 1); printf("left:\n"); print_ast(node->left, indent + 2); }
    if (node->right) { print_indent(indent + 1); printf("right:\n"); print_ast(node->right, indent + 2); }

    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}

void free_ast(ASTNode *node) {
    if (!node) return;
    free(node->name);
    free(node->op);
    free(node->str_value);
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->condition);
    free_ast(node->then_branch);
    free_ast(node->else_branch);
    free_ast(node->body);
    for (int i = 0; i < node->child_count; i++) free_ast(node->children[i]);
    free(node->children);
    free(node);
}
