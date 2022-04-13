#include "ikacc.h"

void	dbg_token(Token *this)
{
	fprintf(stderr, "Start dbg_token\n");

	while (!at_eof(this))
	{
		if (is_same_token_kind(this, TK_NUM))
			fprintf(stderr, "TK_NUM      : %d\n", this->val);
		else if (is_same_token_kind(this, TK_RESERVED))
			fprintf(stderr, "TK_RESERVED : %s\n", this->str);
		else if (is_same_token_kind(this, TK_IDENT))
			fprintf(stderr, "TK_IDENT    : %s\n", this->str);
		fprintf(stderr, "len         : %d\n", this->len);
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
		case ND_MUL:
			fprintf(stderr, "%c\n", '*');
			break ;
		case ND_DIV:
			fprintf(stderr, "%c\n", '/');
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
	// PP(node);
	// fprintf(stderr, "Start dbg_node\n");
	// dbg_node(node);
	// fprintf(stderr, "End dbg_node\n");

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// Prologue
	printf("\tpush rbp\n");
	printf("\tmov rbp, rsp\n");
	printf("\tsub rsp, 208\n"); // Allocate space 26 (variables) * 8 (bit)

	for (int i = 0; code[i] != NULL; i++)
	{
		gen(code[i]);
		printf("\tpop rax\n");
	}

	// Epilogue
	printf("\tmov rsp, rbp\n");
	printf("\tpop rbp\n");
	
	printf("\tret\n");
	return (0);
}
