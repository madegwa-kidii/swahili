#include <stdio.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/codegen.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Matumizi: swahilic <file.swa>\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("fopen"); return 1; }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(size + 1);
    fread(src, 1, size, f);
    src[size] = '\0';
    fclose(f);

    int tok_count;
    Token *tokens = tokenize(src, &tok_count);
    Node *ast = parse(tokens);

    generate_asm(ast, "build/out.s");

	// on linux

    system("nasm -felf64 build/out.s -o build/out.o");
	system("gcc build/out.o -o build/out -no-pie");


	// on windows
	// system("nasm -f win64 build/out.s -o build/out.o");
	// system("gcc build/out.o -o build/out.exe");


    printf("Code ime build sasa: ./build/out\n");
    return 0;
}
