#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

typedef enum e_TokenKind
{
	TK_RESERVED,	// 記号
	TK_NUM,			// 整数トークン
	TK_EOF,			// 入力の終わりを表すトークン
}	TokenKind;

typedef struct Token Token;
// トークン型
struct Token
{
	TokenKind	kind;	// トークンの型
	Token		*next;	// 次の入力トークン
	int			val;	// kindがTK_NUMの場合、その数値
	char		*str;	// トークン文字列
};

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

Token	*new_token(Token *old)
{
	old->next = calloc(1, sizeof(Token));
	return (old->next);
}

void	set_kind(Token *this, TokenKind kind)
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

	// fprintf(stderr, "Start tokenize\n");
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
			set_kind(cur, TK_NUM);
			set_str(cur, code);
			set_val(cur, strtol(code, &code, 10));
			continue ;
		}
		if (*code == '+' || *code == '-')
		{
			cur = new_token(cur);
			set_kind(cur, TK_RESERVED);
			set_str(cur, code);
			code++;
			continue ;
		}
		error("error: Cannot tokenize '%s'\n", code);
		return (NULL);
	}
	cur = new_token(cur);
	set_kind(cur, TK_EOF);
	// fprintf(stderr, "End tokenize\n");
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

int	expect_number(Token **this)
{
	int	n;

	n = get_val(*this);
	*this = (*this)->next;
	return (n);
}

void	codegen(Token *token)
{
	// fprintf(stderr, "Start codegen\n");

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	printf("\tmov rax, %d\n", expect_number(&token));

	while (!at_eof(token))
	{
		if (consume(&token, '+'))
		{
			printf("\tadd rax, %d\n", expect_number(&token));
			continue ;
		}
		if (consume(&token, '-'))
		{
			printf("\tsub rax, %d\n", expect_number(&token));
			continue ;
		}
		error("error: Cannot parse %s\n", token->str);
	}

	printf("ret\n");

	// fprintf(stderr, "End codegen\n");
}

void	dbg(Token *this)
{
	fprintf(stderr, "Start dbg\n");

	while (!at_eof(this))
	{
		if (is(this, TK_NUM))
			fprintf(stderr, "TK_NUM      : %d\n", this->val);
		if (is(this, TK_RESERVED))
			fprintf(stderr, "TK_RESERVED : %c\n", this->str[0]);
		this = this->next;
	}

	fprintf(stderr, "End dbg\n");
	exit(1);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "error: invalid argument\n");
		return (1);
	}

	Token	*token = tokenize(argv[1]);

	// dbg(token);

	codegen(token);
	return (0);
}
