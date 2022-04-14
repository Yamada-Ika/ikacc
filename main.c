#include "ikacc.h"

void	dbg_token(Token *this)
{
	fprintf(stderr, "Start dbg_token\n");

	while (!at_eof(this))
	{
		if (is_same_token_kind(this, TK_NUM))
			fprintf(stderr, "TK_NUM      : %d\n", this->val);
		else if (is_same_token_kind(this, TK_RESERVED))
			fprintf(stderr, "TK_RESERVED : %.*s\n", this->len, this->str);
		else if (is_same_token_kind(this, TK_IDENT))
			fprintf(stderr, "TK_IDENT    : %.*s\n", this->len, this->str);
		else if (is_same_token_kind(this, TK_IF))
			fprintf(stderr, "TK_IF       : if\n");
		else if (is_same_token_kind(this, TK_ELSE))
			fprintf(stderr, "TK_ELSE     : else\n");
		else if (is_same_token_kind(this, TK_FOR))
			fprintf(stderr, "TK_FOR      : for\n");
		else if (is_same_token_kind(this, TK_RETURN))
			fprintf(stderr, "TK_RETURN   : return\n");
		this = this->next;
	}

	fprintf(stderr, "End dbg_token\n");
	// exit(1);
}

void	dbg_node(Node *this)
{
	switch (this->kind) {
		case ND_NUM:
			fprintf(stderr, "%d\n", this->val);
			return ;
		case ND_LVAR:
			fprintf(stderr, "lvar\n");
			return ;
		case ND_ASSIGN:
			fprintf(stderr, "%s\n", "=");
			return ;
	}

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
		case ND_LT:
			fprintf(stderr, "%s\n", "<");
			break ;
		case ND_LE:
			fprintf(stderr, "%s\n", "<=");
			break ;
		case ND_EQ:
			fprintf(stderr, "%s\n", "==");
			break ;
		case ND_NE:
			fprintf(stderr, "%s\n", "<=");
			break ;
	}

	// exit(1);
}

int	count_lvar_num(void)
{
	int	cnt;

	cnt = 0;
	while (locals != NULL)
	{
		cnt++;
		locals = locals->next;
	}
	return cnt;
}

int	allocate_lvar_space(void)
{
	int	cnt_lvar;

	cnt_lvar = count_lvar_num();
	if (cnt_lvar == 0)
		return 0;
	return (cnt_lvar - 1) * 8;
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		error("error: invalid argument");

	// Initialize global vars
	g_code = argv[1];
	locals = (Lvar *)calloc(1, sizeof(Lvar));

	Token	*token = tokenize(argv[1]);
	// dbg_token(token);

	Node	*node = parse(token);
	// PP(node);
	// fprintf(stderr, "Start dbg_node\n");
	// for (int i = 0; code[i] != NULL; i++)
	// {
	// 	dbg_node(code[i]);
	// }
	// fprintf(stderr, "End dbg_node\n");

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// Prologue
	printf("\tpush rbp\n");
	printf("\tmov rbp, rsp\n");
	printf("\tsub rsp, %d\n", allocate_lvar_space()); // Allocate space 26 (variables) * 8 (bit)

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
