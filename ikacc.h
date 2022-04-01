#ifndef IKACC_H
# define IKACC_H

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

// 抽象構文木のノードの種類
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;
// 抽象構文木のノードの型
struct Node {
	NodeKind kind; // ノードの型
	Node *lhs;     // 左辺
	Node *rhs;     // 右辺
	int val;       // kindがND_NUMの場合のみ使う
};

// Global var
char	*g_code;

// tokenize
Token	*tokenize(char *code);
bool	consume(Token **this, char op);
bool	at_eof(Token *this);
bool	is(Token *this, TokenKind kind);
int		expect_number(Token **this);

// parse
Node	*parse(Token *token);

// error
void	error(char *fmt, ...);
void	error_at(char *loc, char *fmt, ...);

#endif
