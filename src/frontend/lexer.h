#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_INT,
    TOK_IDENT,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_EQUAL,
    TOK_SEMI,
    TOK_LPAREN,
    TOK_RPAREN,
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
