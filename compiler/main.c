#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../semantic/semantic.h"
#include "../ir/ir.h"
#include "../optimizer/optimizer.h"
#include "../codegen/codegen.h"

#include <stdio.h>
#include <stdlib.h>

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc((size_t)n + 1);
    if (!buf) exit(1);
    fread(buf, 1, (size_t)n, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Matumizi: swahili_compiler <file.swa>\n");
        return 1;
    }

    char *source = read_file(argv[1]);

    TokenStream tokens = lex_source(source);
    print_tokens(&tokens);

    ASTNode *ast = parse_tokens(&tokens);
    printf("\n=== 2) Syntax Analysis (AST) ===\n");
    print_ast(ast, 0);

    SymbolTable table;
    if (!semantic_check(ast, &table)) {
        st_free(&table);
        free_ast(ast);
        free_tokens(&tokens);
        free(source);
        return 1;
    }

    IRList ir = ir_generate(ast);
    ir_print(&ir, "4) Intermediate Code Generation (TAC)");

    IRList opt = optimize_ir(&ir);
    ir_print(&opt, "5) Optimized IR");

    generate_pseudo_assembly(&opt);

    st_free(&table);
    ir_free(&opt);
    ir_free(&ir);
    free_ast(ast);
    free_tokens(&tokens);
    free(source);

    return 0;
}
