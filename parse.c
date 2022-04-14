
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

void	expect_kind(Token **this, TokenKind kind)
{
	if (!is_same_token_kind(*this, kind))
		error_at((*this)->str, "should be numeric");
	*this = (*this)->next;
}

Lvar	*find_lvar(Token *token)
{
	Lvar	*head;
	Lvar	*at;

	head = locals;
	while (locals->next != NULL)
	{
		if (
			locals->len == token->len
			&& strncmp(locals->name, token->str, locals->len) == 0
		)
		{
			at = locals;
			locals = head;
			return at;
		}
		locals = locals->next;
	}
	locals = head;
	return NULL;
}

Node	*primary(Token **token)
{
	Node	*node;

	// DBG();
	if (consume(token, "("))
	{
		node = expr(token);
		expect(token, ")");
		return node;
	}
	if (is_same_token_kind(*token, TK_NUM))
	{
		node = new_node(NULL, NULL);
		set_node_kind(node, ND_NUM);
		set_node_val(node, expect_number(token));
		return node;
	}
	if (is_same_token_kind(*token, TK_IDENT))
	{
		Token	*tk = *token;
		if (consume_next(token, "("))
		{
			node = new_node(NULL, NULL);
			set_node_kind(node, ND_FUNC);
			node->name = tk->str;
			node->len = tk->len;
			expect(token, ")");
			return node;
		}

		node = new_node(NULL, NULL);
		set_node_kind(node, ND_LVAR);

		Lvar *lvar = find_lvar(*token);
		if (lvar == NULL) // lvarがない
		{
			lvar = (Lvar *)calloc(1, sizeof(Lvar));
			lvar->next = locals;
			lvar->name = (*token)->str;
			lvar->len = (*token)->len;
			lvar->offset = locals->offset + 8;
			node->offset = lvar->offset;
			locals = lvar;
		}
		else // ある
		{
			node->offset = lvar->offset;
		}

		expect_kind(token, TK_IDENT);
		return node;
	}
	error("error: Failed to parse");
}

Node	*unary(Token **token)
{
	Node	*node;

	// DBG();
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

	// DBG();
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

	// DBG();
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

	// DBG();
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
		{
			return node;
		}
	}
	return node;
}

Node	*equality(Token **token)
{
	Node	*node;

	// DBG();
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
		{
			return node;
		}
	}
	return node;
}

Node	*assign(Token **token)
{
	Node	*node;

	// DBG();
	node = equality(token);
	if (consume(token, "="))
	{
		node = new_node(node, assign(token));
		set_node_kind(node, ND_ASSIGN);
	}
	return node;
}

Node	*expr(Token **token)
{
	// DBG();
	return assign(token);
}

// bool	consume_next(Token **this, const char *op)
// {
// 	if (!is_same_token_kind((*this)->next, TK_RESERVED)
// 		|| !is_same_token_str((*this)->next, op))
// 		return (false);
// 	*this = (*this)->next;
// 	return (true);
// }

Vector	*vec_new(void)
{
	Vector	*new;

	new = (Vector *)calloc(1, sizeof(Vector));
	new->data = (void **)calloc(10, sizeof(void *));
	new->capacity = 10;
	new->len = 0;
	return new;
}

void	vec_push(Vector *this, void *data)
{
	// DBG();
	if (this->len == this->capacity - 1)
	{
		this->data = realloc(this->data, sizeof(void *) * (this->capacity + 10));
		this->capacity += 10;
	}
	this->data[this->len] = data;
	this->len++;
}

Node	*stmt(Token **token)
{
	Node	*node;

	// DBG();
	if (consume(token, "{"))
	{
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_BLOCK);
		node->stmts = vec_new();
		while (!consume(token, "}"))
		{
			vec_push(node->stmts, stmt(token));
		}
	}
	else if (consume_kind(token, TK_IF))
	{
		expect(token, "(");
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_IF);
		node->cond = expr(token);
		expect(token, ")");
		node->then = stmt(token);
		if (consume_kind(token, TK_ELSE))
			node->els = stmt(token);
	}
	else if (consume_kind(token, TK_WHILE))
	{
		expect(token, "(");
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_WHILE);
		node->cond = expr(token);
		expect(token, ")");
		node->then = stmt(token);
	}
	else if (consume_kind(token, TK_FOR))
	{
		expect(token, "(");
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_FOR);
		if (!consume(token, ";"))
		{
			node->init = expr(token);
			expect(token, ";");
		}
		if (!consume(token, ";"))
		{
			node->cond = expr(token);
			expect(token, ";");
		}
		if (!consume(token, ")"))
		{
			node->step = expr(token);
			expect(token, ")");
		}
		node->then = stmt(token);
	}
	else if (consume_kind(token, TK_RETURN))
	{
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_RETURN);
		node->lhs = expr(token);
		expect(token, ";");
	}
	else
	{
		node = expr(token);
		expect(token, ";");
	}
	return node;
}

Node	*program(Token **token)
{
	size_t	i;

	// DBG();
	i = 0;
	while (!at_eof(*token))
	{
		code[i] = stmt(token);
		i++;
	}
	code[i] = NULL;
	return code[0];
}

Node	*parse(Token *token)
{
	// DBG();
	return (program(&token));
}
