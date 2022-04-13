#ifndef IKACC_H
# define IKACC_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

# define DBG() fprintf(stderr, "%s %d\n", __func__, __LINE__)
# define PS(s) fprintf(stderr, "%s\n", s)
# define PP(p) fprintf(stderr, "%p\n", p)
# define PD(d) fprintf(stderr, "%d\n", d)

typedef enum e_TokenKind
{
	TK_RESERVED,	// +,-,*,/,(,),<,>,<=,>=,==,!=
	TK_IDENT,		// identifer
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
	int			len;	// length of token
};

// 抽象構文木のノードの種類
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // <
	ND_LE, // <=
	ND_ASSIGN, // =
	ND_LVAR, // local var
	ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;
// 抽象構文木のノードの型
struct Node {
	NodeKind kind; // ノードの型
	Node *lhs;     // 左辺
	Node *rhs;     // 右辺
	int val;       // kindがND_NUMの場合のみ使う
	int offset;    // kindがND_LVARの場合のみ使う
};

// Global var
char	*g_code;
Node	*code[100];

// tokenize
Token	*tokenize(char *code);
bool	consume(Token **this, char *op);
void	expect(Token **this, char *op);
bool	at_eof(Token *this);
bool	is_same_token_kind(Token *this, TokenKind kind);
int		expect_number(Token **this);

// parse
Node	*parse(Token *token);

// codegen
void	gen(Node *node);

// error
void	error(char *fmt, ...);
void	error_at(char *loc, char *fmt, ...);

#endif
