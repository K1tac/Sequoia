#define _GNU_SOURCE
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"

Token make(TokenType t, const char *txt) {
    Token tok;
    tok.type = t;
    tok.text = strdup(txt);
    return tok;
}

char peek(Lexer *l) {
    return l->src[l->pos];
}

char advance(Lexer *l) {
    return l->src[l->pos++];
}

Token next_token(Lexer *l) {
    while (isspace(peek(l))) advance(l);

    char c = peek(l);

    if (isdigit(c)) {
        int start = l->pos;
        while (isdigit(peek(l))) advance(l);
        int len = l->pos - start;
        char *buf = strndup(l->src + start, len);
        return make(TOK_INT, buf);
    }

    if (isalpha(c)) {
        int start = l->pos;
        while (isalnum(peek(l)) || peek(l) == '_') advance(l);
        int len = l->pos - start;
        char *buf = strndup(l->src + start, len);
        return make(TOK_IDENT, buf);
    }

    switch (c) {
        case '+': advance(l); return make(TOK_PLUS, "+");
        case '-': advance(l); return make(TOK_MINUS, "-");
        case '*': advance(l); return make(TOK_STAR, "*");
        case '/': advance(l); return make(TOK_SLASH, "/");
        case '=': advance(l); return make(TOK_EQUAL, "=");
        case ';': advance(l); return make(TOK_SEMI, ";");
        case '(': advance(l); return make(TOK_LPAREN, "(");
        case ')': advance(l); return make(TOK_RPAREN, ")");
        case '\0': return make(TOK_EOF, "");
        default: advance(l); return make(TOK_EOF, "");
    }
}

Lexer *lexer_new(const char *src) {
    Lexer *l = malloc(sizeof(Lexer));
    l->src = src;
    l->pos = 0;
    l->current = next_token(l);
    return l;
}

Token lexer_peek(Lexer *l) {
    return l->current;
}

Token lexer_next(Lexer *l) {
    Token current = l->current;
    l->current = next_token(l);
    return current;
}

void lexer_free(Lexer *l) {
    if (l->current.text) free(l->current.text);
    free(l);
}
