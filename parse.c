
#include "ikacc.h"

void	set_node_kind(Node *this, NodeKind kind)
{
	this->kind = kind;
}

void	set_node_val(Node *this, int val)
{
	this->val = val;
}

Node	*new_node(Node *lhs, Node *rhs)
{
	Node	*new;

	new = calloc(1, sizeof(Node));
	new->lhs = lhs;
	new->rhs = rhs;
	return new;
}

Node	*primary(Token **token)
{
	Node	*node;

	node = new_node(NULL, NULL);
	set_node_kind(node, ND_NUM);
	set_node_val(node, expect_number(token));
	return node;
}

Node	*mul(Token **token)
{
	// Node	*node;

	return (primary(token));
}

Node	*expr(Token **token)
{
	Node	*node;

	node = mul(token);
	while (true)
	{
		if (consume(token, '+'))
		{
			node = new_node(node, mul(token));
			set_node_kind(node, ND_ADD);
		}
		else if (consume(token, '-'))
		{
			node = new_node(node, mul(token));
			set_node_kind(node, ND_SUB);
		}
		else
		{
			return node;
		}
	}
}

Node	*parse(Token *token)
{
	return (expr(&token));
}
