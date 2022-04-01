
#include "ikacc.h"

Token	*new_token(Token *old)
{
	old->next = calloc(1, sizeof(Token));
	return (old->next);
}

void	set_token_kind(Token *this, TokenKind kind)
{
	this->kind = kind;
}

void	set_val(Token *this, int val)
{
	this->val = val;
}

int	get_val(Token *this)
{
	return (this->val);
}

void	set_str(Token *this, char *str)
{
	this->str = str;
}

Token	*tokenize(char *code)
{
	Token	head;
	Token	*cur;

	cur = &head;
	while (code != NULL)
	{
		while (*code == ' ')
			code++;
		if (*code == '\0')
			break ;
		if (isdigit(*code))
		{
			cur = new_token(cur);
			set_token_kind(cur, TK_NUM);
			set_str(cur, code);
			set_val(cur, strtol(code, &code, 10));
			continue ;
		}
		if (*code == '+' || *code == '-' || *code == '*' || *code == '/' || *code == '(' || *code == ')')
		{
			cur = new_token(cur);
			set_token_kind(cur, TK_RESERVED);
			set_str(cur, code);
			code++;
			continue ;
		}
		error_at(code, "Cannot tokenize");
		return (NULL);
	}
	cur = new_token(cur);
	set_token_kind(cur, TK_EOF);
	return (head.next);
}

bool	is(Token *this, TokenKind kind)
{
	return (this->kind == kind);
}

bool	at_eof(Token *this)
{
	return (is(this, TK_EOF));
}

bool	consume(Token **this, char op)
{
	if (!is(*this, TK_RESERVED) || (*this)->str[0] != op)
		return (false);
	*this = (*this)->next;
	return (true);
}

void	expect(Token **this, char op)
{
	if (!is(*this, TK_RESERVED) || (*this)->str[0] != op)
		error_at((*this)->str, "expect %c, but got %c\n", op, (*this)->str[0]);
	*this = (*this)->next;
}

int	expect_number(Token **this)
{
	int	n;

	if (!is(*this, TK_NUM))
	{
		error_at((*this)->str, "should be numeric");
	}
	n = get_val(*this);
	*this = (*this)->next;
	return (n);
}
