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
	}

	printf("\tpush rax\n");

	// fprintf(stderr, "End gen\n");
}

void	dbg_token(Token *this)
{
	fprintf(stderr, "Start dbg_token\n");

	while (!at_eof(this))
	{
		if (is(this, TK_NUM))
			fprintf(stderr, "TK_NUM      : %d\n", this->val);
		if (is(this, TK_RESERVED))
			fprintf(stderr, "TK_RESERVED : %c\n", this->str[0]);
		this = this->next;
	}

	fprintf(stderr, "End dbg_token\n");
	// exit(1);
}

void	dbg_node(Node *this)
{
	if (this->kind == ND_NUM)
	{
		fprintf(stderr, "%d\n", this->val);
		return ;
	}
	if (this == NULL)
		return ;
	dbg_node(this->lhs);
	dbg_node(this->rhs);

	switch (this->kind) {
		case ND_ADD:
			fprintf(stderr, "%c\n", '+');
			break ;
		case ND_SUB:
			fprintf(stderr, "%c\n", '-');
			break ;
	}

	// exit(1);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		error("error: invalid argument");

	g_code = argv[1];
	Token	*token = tokenize(argv[1]);
	// dbg_token(token);

	Node	*node = parse(token);
	// fprintf(stderr, "Start dbg_node\n");
	// dbg_node(node);
	// fprintf(stderr, "End dbg_node\n");

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	gen(node);
	printf("\tpop rax\n");
	printf("\tret\n");
	return (0);
}
