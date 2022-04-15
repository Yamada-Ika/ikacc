#include "ikacc.h"

int	label;

void	gen_lvar(Node *node)
{
	if (node->kind != ND_LVAR)
		error("error: Invalid assign");
	
	printf("\tmov rax, rbp\n");
	printf("\tsub rax, %d\n", node->offset);
	printf("\tpush rax\n");
}

void	gen(Node *node)
{
	// fprintf(stderr, "Start gen\n");

	if (node == NULL)
		return ;

	switch (node->kind) {
		case ND_FUNCDECL: {
			printf("%.*s:\n", node->len, node->name);
			printf("\tpush rbp\n");
			printf("\tmov rbp, rsp\n");
			printf("\tsub rsp, %d\n", allocate_lvar_space());
			for (int i = 0; i < node->args->len; i++) {
				gen_lvar(node->args->data[i]); // varのアドレスがスタックトップに置かれる
				printf("\tpop rax  # %d\n", __LINE__);
				switch (i) {
				case 0:
					printf("\tmov [rax], rdi\n");
					break ;
				case 1:
					printf("\tmov [rax], rsi\n");
					break;
				case 2:
					printf("\tmov [rax], rdx\n");
					break;
				case 3:
					printf("\tmov [rax], rcx\n");
					break;
				case 4:
					printf("\tmov [rax], r8\n");
					break;
				case 5:
					printf("\tmov [rax], r9\n");
					break;
				}
			}
			gen(node->body);
			// printf("\tpop rax  # %d\n", __LINE__);
			printf("\tmov rsp, rbp\n");
			printf("\tpop rbp\n");
			printf("\tret\n");
			return ;
		}
		case ND_BLOCK: {
			for (int i = 0; i < node->stmts->len; i++) {
				gen(node->stmts->data[i]);
				printf("\tpop rax  # %d\n", __LINE__); // return がないとどんどんスタックに値が積まれるのでpopしておく
			}
			return ;
		}
		case ND_FUNCCALL: {
			for (int i = 0; i < node->args->len; i++) {
				switch (i) {
				case 0:
					gen(node->args->data[0]);
					printf("\tpop rdi\n");
					break;
				case 1:
					gen(node->args->data[1]);
					printf("\tpop rsi\n");
					break;
				case 2:
					gen(node->args->data[2]);
					printf("\tpop rdx\n");
					break;
				case 3:
					gen(node->args->data[3]);
					printf("\tpop rcx\n");
					break;
				case 4:
					gen(node->args->data[4]);
					printf("\tpop r8\n");
					break;
				case 5:
					gen(node->args->data[5]);
					printf("\tpop r9\n");
					break;
				}
			}
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
		case ND_LVAR: {
			gen_lvar(node);
			printf("\tpop rax  # %d\n", __LINE__);
			printf("\tmov rax, [rax]\n");
			printf("\tpush rax\n");
			return ;
		}
		case ND_ASSIGN: {
			gen_lvar(node->lhs);
			gen(node->rhs);
			printf("\tpop rdi\n");
			printf("\tpop rax  # %d\n", __LINE__);
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
