
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

void	set_token_len(Token *this, int len)
{
	this->len = len;
}

Token	*new_token_num(Token *old, char **code)
{
	Token	*new;
	char	*end;

	new = new_token(old);
	set_token_kind(new, TK_NUM);
	set_str(new, *code);
	set_val(new, strtol(*code, &end, 10));
	set_token_len(new, end - *code);
	*code = end;
	return (new);
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
			cur = new_token_num(cur, &code);
			continue ;
		}
		if (*code == '+' || *code == '-' || *code == '*' || *code == '/' || *code == '(' || *code == ')')
		{
			cur = new_token(cur);
			set_token_kind(cur, TK_RESERVED);
			set_str(cur, code);
			set_token_len(cur, 1);
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

bool	is_same_token_kind(Token *this, TokenKind kind)
{
	return (this->kind == kind);
}

bool	at_eof(Token *this)
{
	return (is_same_token_kind(this, TK_EOF));
}

bool	is_same_len(char *s, int len)
{
	return (strlen(s) == len);
}

bool	is_same_token_str(Token *this, char *s)
{
	return (strncmp(this->str, s, this->len) == 0
		&& is_same_len(s, this->len));
}

bool	consume(Token **this, char *op)
{
	if (!is_same_token_kind(*this, TK_RESERVED)
		|| !is_same_token_str(*this, op))
		return (false);
	*this = (*this)->next;
	return (true);
}

void	expect(Token **this, char *op)
{
	if (!is_same_token_kind(*this, TK_RESERVED)
		|| !is_same_token_str(*this, op))
		error_at((*this)->str, "expect %c, but got %c\n", op, (*this)->str[0]);
	*this = (*this)->next;
}

int	expect_number(Token **this)
{
	int	n;

	if (!is_same_token_kind(*this, TK_NUM))
	{
		error_at((*this)->str, "should be numeric");
	}
	n = get_val(*this);
	*this = (*this)->next;
	return (n);
}
