#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <string.h>
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

static int is_expr_start(TokenType type) {
    return type == TOK_INT ||
           type == TOK_STRING ||
           type == TOK_IDENT ||
           type == TOK_LPAREN;
}

static AstExpr *parse_expr(Parser *p);
AstStmt *parse_stmt(Parser *p);

static AstExpr *parse_primary(Parser *p) {
    if (check(p, TOK_INT)) {
        Token tok = p->current;
        advance(p);
        return ast_literal(tok.text);
    }
    
    if (check(p, TOK_STRING)) {
        Token tok = p->current;
        advance(p);
        return ast_string_literal(tok.text);
    }
    
    if (check(p, TOK_IDENT)) {
        Token tok = p->current;
        advance(p);
        
        if (match(p, TOK_LPAREN)) {
            AstExpr **args = NULL;
            int arg_count = 0;
            
            if (!check(p, TOK_RPAREN)) {
                int capacity = 10;
                args = malloc(sizeof(AstExpr *) * capacity);
                
                args[arg_count++] = parse_expr(p);
                while (match(p, TOK_COMMA)) {
                    if (arg_count >= capacity) {
                        capacity *= 2;
                        args = realloc(args, sizeof(AstExpr *) * capacity);
                    }
                    args[arg_count++] = parse_expr(p);
                }
            }
            
            match(p, TOK_RPAREN);
            return ast_call(tok.text, args, arg_count);
        }
        
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

static AstExpr *parse_additive(Parser *p) {
    AstExpr *left = parse_term(p);
    
    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
        Operator op = (p->current.type == TOK_PLUS) ? OP_ADD : OP_SUB;
        advance(p);
        AstExpr *right = parse_term(p);
        left = ast_binary(left, op, right);
    }
    
    return left;
}

static AstExpr *parse_comparison(Parser *p) {
    AstExpr *left = parse_additive(p);
    
    if (check(p, TOK_LT) || check(p, TOK_GT) || check(p, TOK_LE) || 
        check(p, TOK_GE) || check(p, TOK_EQ) || check(p, TOK_NE)) {
        
        Operator op;
        switch (p->current.type) {
            case TOK_LT: op = OP_LT; break;
            case TOK_GT: op = OP_GT; break;
            case TOK_LE: op = OP_LE; break;
            case TOK_GE: op = OP_GE; break;
            case TOK_EQ: op = OP_EQ; break;
            case TOK_NE: op = OP_NE; break;
            default: return left;
        }
        advance(p);
        AstExpr *right = parse_additive(p);
        left = ast_binary(left, op, right);
    }
    
    return left;
}

static AstExpr *parse_expr(Parser *p) {
    return parse_comparison(p);
}

static AstStmt **parse_block(Parser *p, int *stmt_count) {
    AstStmt **stmts = NULL;
    int capacity = 10;
    *stmt_count = 0;
    stmts = malloc(sizeof(AstStmt *) * capacity);
    
    if (!match(p, TOK_LBRACE)) {
        free(stmts);
        return NULL;
    }
    
    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        if (*stmt_count >= capacity) {
            capacity *= 2;
            stmts = realloc(stmts, sizeof(AstStmt *) * capacity);
        }
        stmts[(*stmt_count)++] = parse_stmt(p);
    }
    
    match(p, TOK_RBRACE);
    return stmts;
}

AstStmt *parse_stmt(Parser *p) {
    if (match(p, TOK_PRINT)) {
        AstExpr **exprs = NULL;
        int expr_count = 0;
        int capacity = 10;
        exprs = malloc(sizeof(AstExpr *) * capacity);
        
        while (!check(p, TOK_SEMI) && !check(p, TOK_EOF)) {
            if (!is_expr_start(p->current.type)) {
                break;
            }

            if (expr_count >= capacity) {
                capacity *= 2;
                exprs = realloc(exprs, sizeof(AstExpr *) * capacity);
            }

            exprs[expr_count++] = parse_expr(p);

            if (match(p, TOK_COMMA)) {
                continue;
            }
        }
        
        match(p, TOK_SEMI);
        return ast_print(exprs, expr_count);
    }
    
    if (match(p, TOK_RETURN)) {
        AstExpr *expr = NULL;
        if (!check(p, TOK_SEMI)) {
            expr = parse_expr(p);
        }
        match(p, TOK_SEMI);
        return ast_return(expr);
    }

    if (match(p, TOK_FUNC)) {
        if (!check(p, TOK_IDENT)) {
            return ast_expr_stmt(ast_literal("0"));
        }
        
        Token name_tok = p->current;
        advance(p);
        
        char **params = NULL;
        int param_count = 0;
        
        if (match(p, TOK_LPAREN)) {
            if (!check(p, TOK_RPAREN)) {
                int capacity = 10;
                params = malloc(sizeof(char *) * capacity);
                
                if (check(p, TOK_IDENT)) {
                    params[param_count++] = strdup(p->current.text);
                    advance(p);
                }
                
                while (match(p, TOK_COMMA)) {
                    if (param_count >= capacity) {
                        capacity *= 2;
                        params = realloc(params, sizeof(char *) * capacity);
                    }
                    if (check(p, TOK_IDENT)) {
                        params[param_count++] = strdup(p->current.text);
                        advance(p);
                    }
                }
            }
            match(p, TOK_RPAREN);
        }
        
        int body_count = 0;
        AstStmt **body = parse_block(p, &body_count);
        
        return ast_func_def(name_tok.text, params, param_count, body, body_count);
    }
    
    if (match(p, TOK_IF)) {
        if (!match(p, TOK_LPAREN)) {
            return ast_expr_stmt(ast_literal("0"));
        }
        
        AstExpr *cond = parse_expr(p);
        if (!match(p, TOK_RPAREN)) {
            ast_free_expr(cond);
            return ast_expr_stmt(ast_literal("0"));
        }
        
        int then_count = 0;
        AstStmt **then_body = parse_block(p, &then_count);
        
        AstStmt **else_body = NULL;
        int else_count = 0;
        
        if (match(p, TOK_ELSE)) {
            if (check(p, TOK_IF)) {
                else_count = 1;
                else_body = malloc(sizeof(AstStmt *));
                else_body[0] = parse_stmt(p);
            } else {
                else_body = parse_block(p, &else_count);
            }
        }
        
        return ast_if(cond, then_body, then_count, else_body, else_count);
    }

    if (match(p, TOK_WHILE)) {
        if (!match(p, TOK_LPAREN)) {
            return ast_expr_stmt(ast_literal("0"));
        }

        AstExpr *cond = parse_expr(p);
        if (!match(p, TOK_RPAREN)) {
            ast_free_expr(cond);
            return ast_expr_stmt(ast_literal("0"));
        }

        int body_count = 0;
        AstStmt **body = parse_block(p, &body_count);

        return ast_while(cond, body, body_count);
    }
    
    if (check(p, TOK_IDENT)) {
        Token name_tok = p->current;
        advance(p);
        
        if (match(p, TOK_EQUAL)) {
            AstExpr *expr = parse_expr(p);
            match(p, TOK_SEMI);
            return ast_assign(name_tok.text, expr);
        } else {
            AstExpr *expr;

            if (match(p, TOK_LPAREN)) {
                AstExpr **args = NULL;
                int arg_count = 0;
                int capacity = 10;

                if (!check(p, TOK_RPAREN)) {
                    args = malloc(sizeof(AstExpr *) * capacity);

                    args[arg_count++] = parse_expr(p);
                    while (match(p, TOK_COMMA)) {
                        if (arg_count >= capacity) {
                            capacity *= 2;
                            args = realloc(args, sizeof(AstExpr *) * capacity);
                        }
                        args[arg_count++] = parse_expr(p);
                    }
                }

                match(p, TOK_RPAREN);
                expr = ast_call(name_tok.text, args, arg_count);
            } else {
                expr = ast_identifier(name_tok.text);
            }

            match(p, TOK_SEMI);
            return ast_expr_stmt(expr);
        }
    }

    AstExpr *expr = parse_expr(p);
    match(p, TOK_SEMI);
    return ast_expr_stmt(expr);
}

Program *parse_program(Parser *p) {
    Program *prog = malloc(sizeof(Program));
    prog->stmts = NULL;
    prog->count = 0;
    
    int capacity = 10;
    prog->stmts = malloc(sizeof(AstStmt *) * capacity);
    
    while (!check(p, TOK_EOF)) {
        if (prog->count >= capacity) {
            capacity *= 2;
            prog->stmts = realloc(prog->stmts, sizeof(AstStmt *) * capacity);
        }
        prog->stmts[prog->count++] = parse_stmt(p);
    }
    
    return prog;
}

void parser_free(Parser *p) {
    if (p->lexer) lexer_free(p->lexer);
    free(p);
}

void program_free(Program *prog) {
    if (prog && prog->stmts) {
        for (int i = 0; i < prog->count; i++) {
            ast_free_stmt(prog->stmts[i]);
        }
        free(prog->stmts);
    }
    free(prog);
}
