
#include "ikacc.h"

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

void	set_token_str(Token *this, char *str)
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

	new = (Token *)calloc(1, sizeof(Token));
	old->next = new;
	set_token_kind(new, TK_NUM);
	set_token_str(new, *code);
	set_val(new, strtol(*code, &end, 10));
	set_token_len(new, end - *code);
	*code = end;
	return (new);
}

Token	*new_token(Token *old, TokenKind kind, char **code, int len)
{
	Token	*new;

	new = (Token *)calloc(1, sizeof(Token));
	old->next = new;
	set_token_kind(new, kind);
	set_token_str(new, *code);
	set_token_len(new, len);
	(*code) += len;
	return (new);
}

bool	start_with(const char *s1, const char *s2)
{
	return (strncmp(s1, s2, strlen(s2)) == 0);
}

bool	is_ident_char(const char c)
{
	return (
		isalpha(c)
		|| c == '_'
	);
}

int	calc_ident_len(const char *code)
{
	int	len;

	len = 0;
	while (is_ident_char(*code))
	{
		len++;
		code++;
	}
	return len;
}

bool	is_keyword(const char *code, const char *key)
{
	size_t	len;

	len = strlen(key);
	return (
		start_with(code, key)
		&& !is_ident_char(*(code + len))
		&& !isdigit(*(code + len))
	);
}

Token	*tokenize(char *code)
{
	Token	head;
	Token	*cur;

	cur = &head;
	while (*code != '\0')
	{
		while (*code == ' ')
			code++;
		if (*code == '\0')
			break ;
		if (*code == '\n')
		{
			code++;
			continue ;
		}
		if (*code == '{' || *code == '}')
		{
			cur = new_token(cur, TK_BLOCK, &code, 1);
			continue ;
		}
		if (is_keyword(code, "if"))
		{
			cur = new_token(cur, TK_IF, &code, 2);
			continue ;
		}
		if (is_keyword(code, "else"))
		{
			cur = new_token(cur, TK_ELSE, &code, 4);
			continue ;
		}
		if (is_keyword(code, "while"))
		{
			cur = new_token(cur, TK_WHILE, &code, 5);
			continue ;
		}
		if (is_keyword(code, "for"))
		{
			cur = new_token(cur, TK_FOR, &code, 3);
			continue ;
		}
		if (is_keyword(code, "return"))
		{
			cur = new_token(cur, TK_RETURN, &code, 6);
			continue ;
		}
		if (is_ident_char(*code))
		{
			cur = new_token(cur, TK_IDENT, &code, calc_ident_len(code));
			continue ;
		}
		if (isdigit(*code))
		{
			cur = new_token_num(cur, &code);
			continue ;
		}
		if (start_with(code, "==")
			|| start_with(code, "!=")
			|| start_with(code, "<=")
			|| start_with(code, ">=")
		)
		{
			cur = new_token(cur, TK_RESERVED, &code, 2);
			continue ;
		}
		if (strchr("+-*/()<>;=", *code) != NULL)
		{
			cur = new_token(cur, TK_RESERVED, &code, 1);
			continue ;
		}
		error_at(code, "Cannot tokenize");
		return (NULL);
	}
	cur = new_token(cur, TK_EOF, &code, 0);
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

bool	consume_kind(Token **this, TokenKind kind)
{
	if (!is_same_token_kind(*this, kind))
		return (false);
	*this = (*this)->next;
	return (true);
}

bool	consume(Token **this, char *op)
{
	if (!is_same_token_str(*this, op))
		return (false);
	*this = (*this)->next;
	return (true);
}

void	expect(Token **this, char *op)
{
	if (!is_same_token_str(*this, op))
		error_at((*this)->str, "expect %s, but got %c\n", op, (*this)->str[0]);
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
