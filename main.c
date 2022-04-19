#include "ikacc.h"

void	dump_token(Token *this)
{
	fprintf(stderr, "Start dump_token\n");

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
		else if (is_same_token_kind(this, TK_BLOCK))
			fprintf(stderr, "TK_BLOCK    : %.*s\n", this->len, this->str);
		else if (is_same_token_kind(this, TK_TYPE))
			fprintf(stderr, "TK_TYPE     : %.*s\n", this->len, this->str);
		this = this->next;
	}

	fprintf(stderr, "End dump_token\n");
	exit(1);
}

void	dump_node(Node *this)
{
	if (this == NULL)
		return ;

	switch (this->kind) {
		case ND_FUNCDECL: {
			printf("ND_FUNCDECL : %.*s\n", this->len, this->name);
			dump_node(this->body);
			return ;
		}
		case ND_BLOCK: {
			printf("ND_BLOCK\n");
			for (int i = 0; i < this->stmts->len; i++) {
				dump_node(this->stmts->data[i]);
			}
			return ;
		}
		case ND_FUNCCALL: {
			printf("ND_BLOCK\n");
			return ;
		}
		case ND_IF: {
				printf("ND_IF\n");
				dump_node(this->cond);
				dump_node(this->then);
				if (this->els != NULL)
					dump_node(this->els);
				return ;
			}
		case ND_WHILE: {
				printf("ND_WHILE\n");
				dump_node(this->cond);
				dump_node(this->then);
				return ;
			}
		case ND_FOR: {
				printf("ND_FOR\n");
				dump_node(this->init);
				if (this->cond != NULL)
					dump_node(this->cond);
				dump_node(this->then);
				dump_node(this->step);
				return ;
			}
		case ND_RETURN: {
			printf("ND_RETURN\n");
			dump_node(this->lhs);
			return ;
		}
		case ND_NUM: {
			printf("ND_NUM\n");
			return ;
		}
		case ND_ADDR: {
			printf("ND_ADDR\n");
			return ;
		}
		case ND_DEREF: {
			printf("ND_DEREF\n");
			return ;
		}
		case ND_LVAR: {
			printf("ND_LVAR\n");
			return ;
		}
		case ND_ASSIGN: {
			printf("ND_ASSIGN\n");
			dump_node(this->rhs);
			return ;
		}
	}

	dump_node(this->lhs);
	dump_node(this->rhs);

	switch (this->kind) {
		case ND_ADD:
			printf("ND_ADD\n");
			break ;
		case ND_SUB:
			printf("ND_SUB\n");
			break ;
		case ND_MUL:
			printf("ND_MUL\n");
			break ;
		case ND_DIV:
			printf("ND_DIV\n");
			break ;
		case ND_LT:
			printf("ND_LT\n");
			break ;
		case ND_LE:
			printf("ND_LE\n");
			break ;
		case ND_EQ:
			printf("ND_EQ\n");
			break ;
		case ND_NE:
			printf("ND_NE\n");
			break ;
	}

	// exit(1);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		error("error: invalid argument");

	// Initialize global vars
	g_code = argv[1];
	locals = (Lvar *)calloc(1, sizeof(Lvar));

	Token	*token = tokenize(argv[1]);
	// dump_token(token);

	Vector	*node = parse(token);
	// PP(this);
	// fprintf(stderr, "Start dump_node\n");
	// for (int i = 0; i < node->len; i++)
	// {
	// 	dump_node(nodes->data[i]);
	// }
	// fprintf(stderr, "End dump_node\n");
	// exit(1);

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");

	for (int i = 0; i < nodes->len; i++) {
		gen(nodes->data[i]);
	}

	return (0);
}
