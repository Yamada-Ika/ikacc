
#include "ikacc.h"

Node	*assign(Token **token);
Node	*expr(Token **token);
Node	*stmt(Token **token);
Node	*primary(Token **token);

void	set_node_kind(Node *this, NodeKind kind) {
	this->kind = kind;
}

void	set_node_val(Node *this, int val) {
	this->val = val;
}

Node	*new_node2(Node *lhs, Node *rhs) {
	Node	*new;

	new = calloc(1, sizeof(Node));
	new->lhs = lhs;
	new->rhs = rhs;
	return new;
}

Node	*new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node	*new;

	new = new_node2(lhs, rhs);
	set_node_kind(new, kind);
	return new;
}

Node	*new_node_num(int val) {
	Node	*new;

	new = calloc(1, sizeof(Node));
	set_node_kind(new, ND_NUM);
	set_node_val(new, val);
	return new;
}

int	get_node_num(Node *node) {
	return (node->val);
}

void	expect_kind(Token **this, TokenKind kind) {
	if (!is_same_token_kind(*this, kind))
		error_at((*this)->str, "error: expect token kind %d, but got %d", kind, (*this)->kind);
	*this = (*this)->next;
}

Lvar	*find_lvar(Token *token) {
	Lvar	*head;
	Lvar	*at;

	head = locals;
	while (locals->next != NULL) {
		if (
			locals->len == token->len
			&& strncmp(locals->name, token->str, locals->len) == 0
		) {
			at = locals;
			locals = head;
			return at;
		}
		locals = locals->next;
	}
	locals = head;
	return NULL;
}

Node	*var(Token **token) {
	Node	*node;

	node = new_node(ND_LVAR, NULL, NULL);
	// set_node_kind(node, ND_LVAR);

	Lvar *lvar = find_lvar(*token);
	if (lvar == NULL) {
		lvar = (Lvar *)calloc(1, sizeof(Lvar));
		lvar->next = locals;
		lvar->name = (*token)->str;
		lvar->len = (*token)->len;
		lvar->offset = locals->offset + 8;
		node->offset = lvar->offset;
		locals = lvar;
	}
	else {
		node->offset = lvar->offset;
	}


	expect_kind(token, TK_IDENT);
	return node;
}

Node	*funcCall(Token **token) {
	Node	*node;
	Token	*tk;

	tk = *token;

	expect_kind(token, TK_IDENT);
	expect(token, "(");
	node = new_node(ND_FUNCCALL, NULL, NULL);
	node->name = tk->str;
	node->len = tk->len;

	node->args = vec_new();
	for (int cnt = 0; !consume(token, ")"); cnt++) {
		if (cnt >= 1)
			expect(token, ",");
		if (cnt > 5)
			error_at((*token)->str, "error: Too many arguments");
		vec_push(&node->args, assign(token));
	}
	// vec_dump(node->args);
	return node;
}

Node	*primary(Token **token) {
	Node	*node;

	//
	if (consume(token, "(")) {
		node = expr(token);
		expect(token, ")");
		return node;
	}
	if (is_same_token_kind(*token, TK_NUM)) {
		node = new_node(ND_NUM, NULL, NULL);
		set_node_val(node, expect_number(token));
		return node;
	}
	if (is_same_token_kind(*token, TK_IDENT)) {
		if (is_same_token_str((*token)->next, "(")) {
			return funcCall(token);
		}

		Lvar *lvar = find_lvar(*token);
		if (lvar == NULL) {
			error_at((*token)->str, "error: Variable does not decralation");
		}

		return var(token);
	}
	error_at((*token)->str, "error: Failed to parse");
}

Node	*unary(Token **token) {
	Node	*node;

	//
	if (consume(token, "+"))
		return primary(token);
	if (consume(token, "-")) {
		return new_node(ND_SUB, new_node_num(0), primary(token));
	}
	if (consume(token, "&")) {
		return new_node(ND_ADDR, unary(token), NULL);
	}
	if (consume(token, "*")) {
		return new_node(ND_DEREF, unary(token), NULL);
	}
	return primary(token);
}

Node	*mul(Token **token) {
	Node	*node;

	//
	node = unary(token);
	while (true) {
		if (consume(token, "*")) {
			node = new_node(ND_MUL, node, unary(token));
		}
		else if (consume(token, "/")) {
			node = new_node(ND_DIV, node, unary(token));
		}
		else {
			return node;
		}
	}
}

Node	*add(Token **token) {
	Node	*node;

	//
	node = mul(token);
	while (true) {
		if (consume(token, "+")) {
			node = new_node(ND_ADD, node, mul(token));
		}
		else if (consume(token, "-")) {
			node = new_node(ND_SUB, node, mul(token));
		}
		else {
			return node;
		}
	}
}

Node	*relational(Token **token) {
	Node	*node;

	//
	node = add(token);
	while (true) {
		if (consume(token, "<")) {
			node = new_node(ND_LT, node, add(token));
		}
		else if (consume(token, "<=")) {
			node = new_node(ND_LE, node, add(token));
		}
		else if (consume(token, ">")) {
			node = new_node(ND_LT, add(token), node);
		}
		else if (consume(token, ">=")) {
			node = new_node(ND_LE, add(token), node);
		}
		else {
			return node;
		}
	}
	return node;
}

Node	*equality(Token **token) {
	Node	*node;

	//
	node = relational(token);
	while (true) {
		if (consume(token, "==")) {
			node = new_node(ND_EQ, node, relational(token));
		}
		else if (consume(token, "!=")) {
			node = new_node(ND_NE, node, relational(token));
		}
		else {
			return node;
		}
	}
	return node;
}

Node	*assign(Token **token) {
	Node	*node;

	//
	node = equality(token);
	if (consume(token, "=")) {
		node = new_node(ND_ASSIGN, node, assign(token));
	}
	return node;
}

Type	*new_type(typeKind ty) {
	Type	*new;

	new = calloc(1, sizeof(Type));
	new->ty = ty;
	return new;
}

Node	*expr(Token **token) {
	//
	Node	*node;

	if (is_same_token_str(*token, "int")) {

		expect_kind(token, TK_TYPE);
		Type *type = new_type(INT);
		while (consume(token, "*")) {
			Type *next = new_type(PTR);
			next->ptr_to = type;
			type = next;
		}

		node = var(token);
		node->type = type;

		if (consume(token, "=")) {
			node = new_node(ND_ASSIGN, node, assign(token));
		}
	}
	else {
		node = assign(token);
	}
	return node;
}

Node	*blockBody(Token **token) {
	Node	*node;

	//
	node = calloc(1, sizeof(Node));
	set_node_kind(node, ND_BLOCK);
	while (!consume(token, "}")) {
		vec_push(&node->stmts, stmt(token));
	}
	//
	return node;
}

Node	*stmt(Token **token) {
	Node	*node;

	//

	// if (consume(token, "int")) {
	// 	node = var(token);
	// 	expect(token, ";");
	// }
	if (consume(token, "{")) {
		return blockBody(token);
	}
	else if (consume_kind(token, TK_IF)) {
		expect(token, "(");
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_IF);
		node->cond = expr(token);
		expect(token, ")");
		node->then = stmt(token);
		if (consume_kind(token, TK_ELSE))
			node->els = stmt(token);
	}
	else if (consume_kind(token, TK_WHILE)) {
		expect(token, "(");
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_WHILE);
		node->cond = expr(token);
		expect(token, ")");
		node->then = stmt(token);
	}
	else if (consume_kind(token, TK_FOR)) {
		expect(token, "(");
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_FOR);
		if (!consume(token, ";")) {
			node->init = expr(token);
			expect(token, ";");
		}
		if (!consume(token, ";")) {
			node->cond = expr(token);
			expect(token, ";");
		}
		if (!consume(token, ")")) {
			node->step = expr(token);
			expect(token, ")");
		}
		node->then = stmt(token);
	}
	else if (consume_kind(token, TK_RETURN)) {
		node = calloc(1, sizeof(Node));
		set_node_kind(node, ND_RETURN);
		node->lhs = expr(token);
		expect(token, ";");
	}
	else {
		node = expr(token);
		expect(token, ";");
	}
	return node;
}

Node	*funcBody(Token **token) {
	return blockBody(token);
}

Node	*funcDecl(Token **token) {
	Node	*node;

	expect(token, "int");
	if (is_same_token_kind(*token, TK_IDENT)) {
		node = new_node(ND_FUNCDECL, NULL, NULL);
		node->name = (*token)->str;
		node->len = (*token)->len;

	
		expect_kind(token, TK_IDENT);
		expect(token, "(");
		node->args = vec_new();
		for (int cnt = 0; !consume(token, ")"); cnt++) {
			if (cnt >= 1)
				expect(token, ",");
			if (cnt > 5)
				error_at((*token)->str, "error: Too many arguments");
			expect(token, "int");
			vec_push(&node->args, var(token));
		}
		// vec_dump(node->args);
		if (consume(token, "{"))
			node->body = funcBody(token);
		else
			error("error: function body not found");

		return node;
	}
	error("error: function declaration not found");
}

Vector	*program(Token **token) {
	while (!at_eof(*token)) {
		vec_push(&(nodes), funcDecl(token));
	}
	return nodes;
}

Vector	*parse(Token *token) {
	//
	return (program(&token));
}
