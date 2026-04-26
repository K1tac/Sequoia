#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_INT,
    TOK_STRING,
    TOK_IDENT,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_EQUAL,
    TOK_SEMI,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_COMMA,
    TOK_LT,
    TOK_GT,
    TOK_LE,
    TOK_GE,
    TOK_EQ,
    TOK_NE,
    TOK_PRINT,
    TOK_FUNC,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *text;
} Token;

typedef struct {
    const char *src;
    int pos;
    Token current;
} Lexer;

Lexer *lexer_new(const char *src);
Token lexer_peek(Lexer *l);
Token lexer_next(Lexer *l);
void lexer_free(Lexer *l);

#endif
