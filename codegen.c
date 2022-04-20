#include "ikacc.h"

int	label;

static int	count_lvar_num(void) {
	int	cnt;
	Lvar	*head;

	head = locals;
	cnt = 0;
	while (locals != NULL)
	{
		cnt++;
		locals = locals->next;
	}
	locals = head;
	return cnt;
}

static int	allocate_lvar_space(void) {
	int	cnt_lvar;

	cnt_lvar = count_lvar_num();
	if (cnt_lvar == 0)
		return 0;
	return (cnt_lvar - 1) * 8;
}

static void	align_stack(Node *node) {
	int align_label = label;
	label++;
	printf("\tmov rax, rsp\n");
	printf("\tand rax, 15 # equivalent to rax %% 16\n");
	printf("\tjnz .L.call.with.align.%d\n", align_label);

	printf("\tcall %.*s\n", node->len, node->name);
	printf("\tjmp .L.end.%d\n", align_label);

	printf(".L.call.with.align.%d:\n", align_label);
	printf("\tsub rsp, 8\n");
	printf("\tcall %.*s\n", node->len, node->name);
	printf("\tadd rsp, 8\n");
	printf(".L.end.%d:\n", align_label);
	printf("\tpush rax\n");
}

static void	gen_lvar(Node *node) {
	if (node->kind == ND_LVAR) {
		printf("\tmov rax, rbp\n");
		printf("\tsub rax, %d\n", node->offset);
		printf("\tpush rax\n");
		return ;
	}
	if (node->kind == ND_DEREF) {
		gen(node->lhs);
		return ;
	}
	PD(node->kind);
	error("error: Invalid assign");
}

static int	size_of(Type *type) {
	if (type->ty == PTR)
		return 8;
	if (type->ty == INT)
		return 4;
	error("error: type did not find");
}

void	gen(Node *node) {
	// fprintf(stderr, "Start gen\n");

	if (node == NULL)
		return ;

	switch (node->kind) {
		case ND_FUNCDECL: {
			printf("\n%.*s:\n", node->len, node->name);
			printf("\tpush rbp\n");
			printf("\tmov rbp, rsp\n");
			printf("\tsub rsp, %d\n", allocate_lvar_space());
			for (int i = 0; i < node->args->len; i++) {
				gen_lvar(node->args->data[i]); // varのアドレスがスタックトップに置かれる
				printf("\tpop rax  # %d\n", __LINE__);
				if (i == 0)
					printf("\tmov [rax], rdi\n");
				else if (i == 1)
					printf("\tmov [rax], rsi\n");
				else if (i == 2)
					printf("\tmov [rax], rdx\n");
				else if (i == 3)
					printf("\tmov [rax], rcx\n");
				else if (i == 4)
					printf("\tmov [rax], r8\n");
				else if (i == 5)
					printf("\tmov [rax], r9\n");
			}
			gen(node->body);
			printf("\tmov rsp, rbp\n");
			printf("\tpop rbp\n");
			printf("\tret\n");
			return ;
		}
		case ND_BLOCK: {
			for (int i = 0; i < node->stmts->len; i++) {
				gen(node->stmts->data[i]);
				// printf("\tpop rax  # %d\n", __LINE__); // return がないとどんどんスタックに値が積まれるのでpopしておく
			}
			return ;
		}
		case ND_FUNCCALL: {
			for (int i = 0; i < node->args->len; i++) {
				gen(node->args->data[i]);
				if (i == 0)
					printf("\tpop rdi\n");
				else if (i == 1)
					printf("\tpop rsi\n");
				else if (i == 2)
					printf("\tpop rdx\n");
				else if (i == 3)
					printf("\tpop rcx\n");
				else if (i == 4)
					printf("\tpop r8\n");
				else if (i == 5)
					printf("\tpop r9\n");
			}

			// align_stack(node);

			printf("\tcall %.*s\n", node->len, node->name);
			printf("\tpush rax\n");

			return ;
		}
		case ND_IF: {
			int if_label = label;
			label++;
			gen(node->cond);
			printf("\tpop rax  # %d\n", __LINE__);
			printf("\tcmp rax, 0\n");
			printf("\tje .Lelse%d\n", if_label);
			gen(node->then);
			printf("\tjmp .Lend%d\n", if_label);
			printf(".Lelse%d:\n", if_label);
			if (node->els != NULL)
				gen(node->els);
			printf(".Lend%d:\n", if_label);
			return ;
		}
		case ND_WHILE: {
			int while_label = label;
			label++;
			printf(".Lbegin%d:\n", while_label);
			gen(node->cond);
			printf("\tpop rax  # %d\n", __LINE__);
			printf("\tcmp rax, 0\n");
			printf("\tje .Lend%d\n", while_label);
			gen(node->then);
			printf("\tjmp .Lbegin%d\n", while_label);
			printf(".Lend%d:\n", while_label);
			return ;
		}
		case ND_FOR: {
			int for_label = label;
			label++;
			gen(node->init);
			printf(".Lbegin%d:\n", for_label);
			if (node->cond != NULL)
				gen(node->cond);
			else
				printf("\tpush 1\n");
			printf("\tpop rax  # %d\n", __LINE__);
			printf("\tcmp rax, 0\n");
			printf("\tje .Lend%d\n", for_label);
			gen(node->then);
			gen(node->step);
			printf("\tjmp .Lbegin%d\n", for_label);
			printf(".Lend%d:\n", for_label);
			return ;
		}
		case ND_RETURN: {
			gen(node->lhs);
			printf("\tpop rax  # %d\n", __LINE__);
			printf("\tmov rsp, rbp\n");
			printf("\tpop rbp\n");
			printf("\tret\n");
			return ;
		}
		case ND_NUM: {
			printf("\tpush %d\n", node->val);
			return ;
		}
		case ND_ADDR: {
			gen_lvar(node->lhs);
			return ;
		}
		case ND_DEREF: {
			gen(node->lhs);
			printf("\tpop rax  # ND_DEREF\n");
			printf("\tmov rax, [rax]\n");
			printf("\tpush rax\n");
			return ;
		}
		case ND_LVAR: {
			gen_lvar(node);
			printf("\tpop rax  # ND_LVAR\n");
			printf("\tmov rax, [rax]\n");
			printf("\tpush rax\n");
			return ;
		}
		case ND_ASSIGN: {
			gen_lvar(node->lhs);
			gen(node->rhs);
			printf("\tpop rdi\n");
			printf("\tpop rax  # ND_ASSIGN\n");
			printf("\tmov [rax], rdi\n");
			printf("\tpush rdi\n");
			return ;
		}
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("\tpop rdi\n");
	printf("\tpop rax  # %d\n", __LINE__);
	switch (node->kind) {
		case ND_ADD:
			if (node->lhs != NULL
				&& node->lhs->type != NULL
				&& node->lhs->type->ty == PTR) {
				printf("\tpush rax\n");
				printf("\tmov rax, %d\n", size_of(node->lhs->type->ptr_to));
				printf("\timul rdi, rax\n");
				printf("\tpop rax\n");
			}
			printf("\tadd rax, rdi\n");
			break ;
		case ND_SUB:
			if (node->lhs != NULL
				&& node->lhs->type != NULL
				&& node->lhs->type->ty == PTR) {
				printf("\tpush rax\n");
				printf("\tmov rax, %d\n", size_of(node->lhs->type->ptr_to));
				printf("\timul rdi, rax\n");
				printf("\tpop rax\n");
			}
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
