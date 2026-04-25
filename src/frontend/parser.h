#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    Lexer *lexer;
    Token current;
} Parser;

Parser *parser_new(Lexer *lexer);
AstStmt *parse_stmt(Parser *p);
void parser_free(Parser *p);

#endif
