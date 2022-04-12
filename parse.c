
#include "ikacc.h"

Node	*expr(Token **token);

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

Node	*new_node_num(int val)
{
	Node	*new;

	new = calloc(1, sizeof(Node));
	set_node_kind(new, ND_NUM);
	set_node_val(new, val);
	return new;
}

int	get_node_num(Node *node)
{
	return (node->val);
}

Node	*primary(Token **token)
{
	Node	*node;

	if (consume(token, "("))
	{
		node = expr(token);
		expect(token, ")");
		return node;
	}
	node = new_node(NULL, NULL);
	set_node_kind(node, ND_NUM);
	set_node_val(node, expect_number(token));
	return node;
}

Node	*unary(Token **token)
{
	Node	*node;

	if (consume(token, "+"))
		return primary(token);
	if (consume(token, "-"))
	{
		node = new_node(new_node_num(0), primary(token));
		set_node_kind(node, ND_SUB);
		return node;
	}
	return primary(token);
}

Node	*mul(Token **token)
{
	Node	*node;

	node = unary(token);
	while (true)
	{
		if (consume(token, "*"))
		{
			node = new_node(node, unary(token));
			set_node_kind(node, ND_MUL);
		}
		else if (consume(token, "/"))
		{
			node = new_node(node, unary(token));
			set_node_kind(node, ND_DIV);
		}
		else
		{
			return node;
		}
	}
}

Node	*add(Token **token)
{
	Node	*node;

	node = mul(token);
	while (true)
	{
		if (consume(token, "+"))
		{
			node = new_node(node, mul(token));
			set_node_kind(node, ND_ADD);
		}
		else if (consume(token, "-"))
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

Node	*relational(Token **token)
{
	Node	*node;

	node = add(token);
	while (true)
	{
		if (consume(token, "<"))
		{
			node = new_node(node, add(token));
			set_node_kind(node, ND_LT);
		}
		else if (consume(token, "<="))
		{
			node = new_node(node, add(token));
			set_node_kind(node, ND_LE);
		}
		else if (consume(token, ">"))
		{
			node = new_node(add(token), node);
			set_node_kind(node, ND_LT);
		}
		else if (consume(token, ">="))
		{
			node = new_node(add(token), node);
			set_node_kind(node, ND_LE);
		}
		else
			return node;
	}
	return node;
}

Node	*equality(Token **token)
{
	Node	*node;

	node = relational(token);
	while (true)
	{
		if (consume(token, "=="))
		{
			node = new_node(node, relational(token));
			set_node_kind(node, ND_EQ);
		}
		else if (consume(token, "!="))
		{
			node = new_node(node, relational(token));
			set_node_kind(node, ND_NE);
		}
		else
			return node;
	}
	return node;
}

Node	*expr(Token **token)
{
	return equality(token);
}

Node	*parse(Token *token)
{
	return (expr(&token));
}
