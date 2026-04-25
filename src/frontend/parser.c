#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
#include "ast.h"

Parser *parser_new(Lexer *lexer) {
    Parser *p = malloc(sizeof(Parser));
    p->lexer = lexer;
    p->current = lexer_peek(lexer);
    return p;
}

static void advance(Parser *p) {
    lexer_next(p->lexer);
    p->current = lexer_peek(p->lexer);
}

static int match(Parser *p, TokenType type) {
    if (p->current.type == type) {
        advance(p);
        return 1;
    }
    return 0;
}

static int check(Parser *p, TokenType type) {
    return p->current.type == type;
}

static AstExpr *parse_expr(Parser *p);

static AstExpr *parse_primary(Parser *p) {
    if (check(p, TOK_INT)) {
        Token tok = p->current;
        advance(p);
        return ast_literal(tok.text);
    }
    
    if (check(p, TOK_IDENT)) {
        Token tok = p->current;
        advance(p);
        return ast_identifier(tok.text);
    }
    
    if (match(p, TOK_LPAREN)) {
        AstExpr *expr = parse_expr(p);
        match(p, TOK_RPAREN);
        return expr;
    }
    
    return ast_literal("0");
}

static AstExpr *parse_term(Parser *p) {
    AstExpr *left = parse_primary(p);
    
    while (check(p, TOK_STAR) || check(p, TOK_SLASH)) {
        Operator op = (p->current.type == TOK_STAR) ? OP_MUL : OP_DIV;
        advance(p);
        AstExpr *right = parse_primary(p);
        left = ast_binary(left, op, right);
    }
    
    return left;
}

static AstExpr *parse_expr(Parser *p) {
    AstExpr *left = parse_term(p);
    
    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
        Operator op = (p->current.type == TOK_PLUS) ? OP_ADD : OP_SUB;
        advance(p);
        AstExpr *right = parse_term(p);
        left = ast_binary(left, op, right);
    }
    
    return left;
}

AstStmt *parse_stmt(Parser *p) {
    // Check for assignment: ident = expr ;
    if (check(p, TOK_IDENT)) {
        Token name_tok = p->current;
        advance(p);
        
        if (match(p, TOK_EQUAL)) {
            AstExpr *expr = parse_expr(p);
            match(p, TOK_SEMI);
            return ast_assign(name_tok.text, expr);
        } else {
            // Not an assignment, backtrack is not possible, so create expr stmt
            AstExpr *expr = ast_identifier(name_tok.text);
            match(p, TOK_SEMI);
            return ast_expr_stmt(expr);
        }
    }
    
    // Otherwise it's an expression statement
    AstExpr *expr = parse_expr(p);
    match(p, TOK_SEMI);
    return ast_expr_stmt(expr);
}

void parser_free(Parser *p) {
    free(p);
}
