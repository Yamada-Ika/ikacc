#include "ikacc.h"

void	gen(Node *node)
{
	// fprintf(stderr, "Start gen\n");

	if (node->kind == ND_NUM)
	{
		printf("\tpush %d\n", node->val);
		return ;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("\tpop rdi\n");
	printf("\tpop rax\n");
	switch (node->kind) {
		case ND_ADD:
			printf("\tadd rax, rdi\n");
			break ;
		case ND_SUB:
			printf("\tsub rax, rdi\n");
			break ;
		case ND_MUL:
			printf("\timul rax, rdi\n");
			break ;
		case ND_DIV:
			printf("\tcqo\n");
			printf("\tidiv rdi\n");
			break ;
		case ND_LT:
			printf("\tcmp rax, rdi\n");
			printf("\tsetl al\n");
			printf("\tmovzb rax, al\n");
			break ;
		case ND_LE:
			printf("\tcmp rax, rdi\n");
			printf("\tsetle al\n");
			printf("\tmovzb rax, al\n");
			break ;
		case ND_EQ:
			printf("\tcmp rax, rdi\n");
			printf("\tsete al\n");
			printf("\tmovzb rax, al\n");
			break ;
		case ND_NE:
			printf("\tcmp rax, rdi\n");
			printf("\tsetne al\n");
			printf("\tmovzb rax, al\n");
			break ;
	}

	printf("\tpush rax\n");

	// fprintf(stderr, "End gen\n");
}
