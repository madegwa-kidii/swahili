#include <stdio.h>
#include <stdlib.h>
#include "../include/codegen.h"

static int eval(Node *n) {
    if (n->type == NODE_NUMBER) return atoi(n->value);
    if (n->type == NODE_ADD) return eval(n->left) + eval(n->right);
    return 0;
}

void generate_asm(Node *root, const char *outfile) {
    FILE *f = fopen(outfile, "w");
    if (!f) { perror("fopen"); exit(1); }

    fprintf(f, "section .data\n");
    fprintf(f, "fmt db \"%%d\",10,0\n\n");
    fprintf(f, "section .text\n");
    fprintf(f, "global main\n");
    fprintf(f, "extern printf\n\n");
    fprintf(f, "main:\n");

    if (root->type == NODE_PRINT) {
        int value = eval(root->left);


		/*  on linux

        fprintf(f, "    mov rdi, fmt\n");
        fprintf(f, "    mov rsi, %d\n", value);
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    call printf\n");

			on windows

		fprintf(f, "    mov rcx, fmt\n");
		fprintf(f, "    mov rdx, %d\n", value);
		fprintf(f, "    call printf\n");

		*/



		#ifdef _WIN32
            fprintf(f, "    mov rcx, fmt\n");
    		fprintf(f, "    mov rdx, %d\n", value);
    		fprintf(f, "    call printf\n");
		#else
    		fprintf(f, "    mov rdi, fmt\n");
    		fprintf(f, "    mov rsi, %d\n", value);
    		fprintf(f, "    xor rax, rax\n");
    		fprintf(f, "    call printf\n");
		#endif


    }

    fprintf(f, "    ret\n");
    fclose(f);
}
