#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    AstStmt **stmts;
    int count;
} Program;

typedef struct {
    Lexer *lexer;
    Token current;
} Parser;

Parser *parser_new(Lexer *lexer);
Program *parse_program(Parser *p);
AstStmt *parse_stmt(Parser *p);
void parser_free(Parser *p);
void program_free(Program *prog);

#endif
