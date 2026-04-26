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

TokenType check_keyword(const char *txt) {
    if (strcmp(txt, "print") == 0) return TOK_PRINT;
    if (strcmp(txt, "func") == 0) return TOK_FUNC;
    if (strcmp(txt, "return") == 0) return TOK_RETURN;
    if (strcmp(txt, "if") == 0) return TOK_IF;
    if (strcmp(txt, "else") == 0) return TOK_ELSE;
    if (strcmp(txt, "while") == 0) return TOK_WHILE;
    if (strcmp(txt, "break") == 0) return TOK_BREAK;
    if (strcmp(txt, "continue") == 0) return TOK_CONTINUE;
    return TOK_IDENT;
}

Token next_token(Lexer *l) {
    while (isspace(peek(l))) advance(l);
    
    if (peek(l) == '/' && l->src[l->pos + 1] == '/') {
        while (peek(l) != '\n' && peek(l) != '\0') advance(l);
        return next_token(l);
    }
  
    if (peek(l) == '/' && l->src[l->pos + 1] == '*') {
        advance(l);
        advance(l);
        while (!(peek(l) == '*' && l->src[l->pos + 1] == '/') && peek(l) != '\0') {
            advance(l);
        }
        if (peek(l) == '*') {
            advance(l);
            advance(l);
        }
        return next_token(l); 
    }

    char c = peek(l);

    if (isdigit(c)) {
        int start = l->pos;
        while (isdigit(peek(l))) advance(l);
        int len = l->pos - start;
        char *buf = strndup(l->src + start, len);
        return make(TOK_INT, buf);
    }

    if (c == '"') {
        advance(l); 
        int start = l->pos;
        while (peek(l) != '"' && peek(l) != '\0') {
            if (peek(l) == '\\') advance(l); 
            advance(l);
        }
        int len = l->pos - start;
        char *buf = strndup(l->src + start, len);
        if (peek(l) == '"') advance(l); 
        return make(TOK_STRING, buf);
    }

    if (isalpha(c) || c == '_') {
        int start = l->pos;
        while (isalnum(peek(l)) || peek(l) == '_') advance(l);
        int len = l->pos - start;
        char *buf = strndup(l->src + start, len);
        TokenType type = check_keyword(buf);
        return make(type, buf);
    }

    switch (c) {
        case '+': advance(l); return make(TOK_PLUS, "+");
        case '-': advance(l); return make(TOK_MINUS, "-");
        case '*': advance(l); return make(TOK_STAR, "*");
        case '/': advance(l); return make(TOK_SLASH, "/");
        case '%': advance(l); return make(TOK_PERCENT, "%");
        case '=': 
            advance(l);
            if (peek(l) == '=') {
                advance(l);
                return make(TOK_EQ, "==");
            }
            return make(TOK_EQUAL, "=");
        case ';': advance(l); return make(TOK_SEMI, ";");
        case '(': advance(l); return make(TOK_LPAREN, "(");
        case ')': advance(l); return make(TOK_RPAREN, ")");
        case '{': advance(l); return make(TOK_LBRACE, "{");
        case '}': advance(l); return make(TOK_RBRACE, "}");
        case ',': advance(l); return make(TOK_COMMA, ",");
        case '<':
            advance(l);
            if (peek(l) == '=') {
                advance(l);
                return make(TOK_LE, "<=");
            }
            return make(TOK_LT, "<");
        case '>':
            advance(l);
            if (peek(l) == '=') {
                advance(l);
                return make(TOK_GE, ">=");
            }
            return make(TOK_GT, ">");
        case '!':
            advance(l);
            if (peek(l) == '=') {
                advance(l);
                return make(TOK_NE, "!=");
            }
            return make(TOK_BANG, "!");
        case '&':
            advance(l);
            if (peek(l) == '&') {
                advance(l);
                return make(TOK_AND, "&&");
            }
            return make(TOK_EOF, "");
        case '|':
            advance(l);
            if (peek(l) == '|') {
                advance(l);
                return make(TOK_OR, "||");
            }
            return make(TOK_EOF, "");
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
