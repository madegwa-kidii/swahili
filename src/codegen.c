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
	fprintf(f, "fmt db \"%%d\",10,0\n");

	// assign unique labels to string literals
	int str_count = 0;
	Node *cur = root->left;
	while (cur) {
		if (cur->type == NODE_PRINT && cur->left->type == NODE_STRING) {
			fprintf(f, "str%d db \"%s\",10,0\n", str_count++, cur->left->value);
		}
		cur = cur->right;
	}

	fprintf(f, "\nsection .text\n");
	fprintf(f, "global main\n");
	fprintf(f, "extern printf\n\n");
	fprintf(f, "main:\n");

	cur = root->left;
	str_count = 0;
	while (cur) {
		if (cur->type == NODE_PRINT) {
			// Check if it's a number OR an expression that evaluates to a number
			if (cur->left->type == NODE_NUMBER || cur->left->type == NODE_ADD) {
				int value = eval(cur->left);
				fprintf(f, "    mov rdi, fmt\n");
				fprintf(f, "    mov rsi, %d\n", value);
				fprintf(f, "    xor rax, rax\n");
				fprintf(f, "    call printf\n");
			} else if (cur->left->type == NODE_STRING) {
				fprintf(f, "    mov rdi, str%d\n", str_count++);
				fprintf(f, "    xor rax, rax\n");
				fprintf(f, "    call printf\n");
			}
		}
		cur = cur->right;
	}

	fprintf(f, "    ret\n");
	fclose(f);
}

