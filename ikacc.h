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
# define PLVAR(l) fprintf(stderr, "%.*s\n", l->len, l->name)
# define PTKSTR(t) fprintf(stderr, "%.*s\n", t->len, t->str)

typedef enum e_TokenKind
{
	TK_RESERVED,	// +,-,*,/,(,),<,>,<=,>=,==,!=
	TK_RETURN,		// return
	TK_IF,			// if
	TK_ELSE,		// else
	TK_WHILE,		// while
	TK_FOR,			// for
	TK_BLOCK,		// {,}
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
	ND_FUNC, // function
	ND_LVAR, // local var
	ND_NUM, // 整数
	ND_IF, // if
	ND_ELSE, // else
	ND_WHILE, // while
	ND_FOR, // for
	ND_BLOCK, // {,}
	ND_RETURN, // return
} NodeKind;

typedef struct Vector {
	void	**data;
	int		capacity;
	int		len;
}	Vector;

typedef struct Node Node;
// 抽象構文木のノードの型
struct Node {
	NodeKind kind; // ノードの型
	Node *lhs;     // 左辺
	Node *rhs;     // 右辺
	Node *init;     // initialization for for statement
	Node *step;     // step for for statement
	Node *cond;     // condition
	Node *then;     // then
	Node *els;     // else
	Vector *stmts; // for compound statements
	Vector *args;  // function arguments
	int val;       // kindがND_NUMの場合のみ使う
	int offset;    // kindがND_LVARの場合のみ使う
	char *name;    // kindがND_FUNCの場合のみ使う
	int len;    // kindがND_FUNCの場合のみ使う
};

typedef struct Lvar Lvar;
/**
 * @brief ローカル変数を保持する構造体
 * 
 */
struct Lvar {
	Lvar *next;
	char *name;
	int len;
	int offset;
};

// Global var
char	*g_code;
Vector	*nodes;
Lvar	*locals;

// tokenize
Token	*tokenize(char *code);
bool	consume(Token **this, char *op);
bool	consume_next(Token **this, char *op);
bool	consume_kind(Token **this, TokenKind kind);
void	expect(Token **this, char *op);
bool	at_eof(Token *this);
bool	is_same_token_kind(Token *this, TokenKind kind);
int		expect_number(Token **this);
bool	start_with(const char *s1, const char *s2);

// parse
// Node	*parse(Token *token);
Vector	*parse(Token *token);

// codegen
void	gen(Node *node);

// error
void	error(char *fmt, ...);
void	error_at(char *loc, char *fmt, ...);

// vector
Vector	*vec_new(void);
void	vec_push(Vector **this, void *data);
void	vec_dump(Vector *this);

#endif
