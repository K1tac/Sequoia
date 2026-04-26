#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include "ast.h"

AstExpr *ast_literal(const char *v) {
    AstExpr *e = malloc(sizeof(AstExpr));
    e->kind = EXPR_LITERAL;
    e->data.literal.value = strdup(v);
    return e;
}

AstExpr *ast_string_literal(const char *v) {
    AstExpr *e = malloc(sizeof(AstExpr));
    e->kind = EXPR_STRING_LITERAL;
    e->data.str_literal.str_value = strdup(v);
    return e;
}

AstExpr *ast_identifier(const char *n) {
    AstExpr *e = malloc(sizeof(AstExpr));
    e->kind = EXPR_IDENTIFIER;
    e->data.identifier.name = strdup(n);
    return e;
}

AstExpr *ast_binary(AstExpr *l, Operator op, AstExpr *r) {
    AstExpr *e = malloc(sizeof(AstExpr));
    e->kind = EXPR_BINARY;
    e->data.binary.left = l;
    e->data.binary.op = op;
    e->data.binary.right = r;
    return e;
}

AstExpr *ast_call(const char *name, AstExpr **args, int arg_count) {
    AstExpr *e = malloc(sizeof(AstExpr));
    e->kind = EXPR_CALL;
    e->data.call.name = strdup(name);
    e->data.call.args = args;
    e->data.call.arg_count = arg_count;
    return e;
}

AstStmt *ast_expr_stmt(AstExpr *e) {
    AstStmt *s = malloc(sizeof(AstStmt));
    s->kind = STMT_EXPR;
    s->data.expr = e;
    return s;
}

AstStmt *ast_assign(const char *n, AstExpr *v) {
    AstStmt *s = malloc(sizeof(AstStmt));
    s->kind = STMT_ASSIGN;
    s->data.assign.name = strdup(n);
    s->data.assign.value = v;
    return s;
}

AstStmt *ast_print(AstExpr **exprs, int expr_count) {
    AstStmt *s = malloc(sizeof(AstStmt));
    s->kind = STMT_PRINT;
    s->data.print.exprs = exprs;
    s->data.print.expr_count = expr_count;
    return s;
}

AstStmt *ast_return(AstExpr *expr) {
    AstStmt *s = malloc(sizeof(AstStmt));
    s->kind = STMT_RETURN;
    s->data.ret = expr;
    return s;
}

AstStmt *ast_func_def(const char *name, char **params, int param_count, AstStmt **body, int body_count) {
    AstStmt *s = malloc(sizeof(AstStmt));
    s->kind = STMT_FUNC_DEF;
    s->data.func.name = strdup(name);
    s->data.func.params = params;
    s->data.func.param_count = param_count;
    s->data.func.body = body;
    s->data.func.body_count = body_count;
    return s;
}

AstStmt *ast_if(AstExpr *cond, AstStmt **then_body, int then_count, AstStmt **else_body, int else_count) {
    AstStmt *s = malloc(sizeof(AstStmt));
    s->kind = STMT_IF;
    s->data.if_stmt.cond = cond;
    s->data.if_stmt.then_body = then_body;
    s->data.if_stmt.then_count = then_count;
    s->data.if_stmt.else_body = else_body;
    s->data.if_stmt.else_count = else_count;
    return s;
}

AstStmt *ast_block(AstStmt **stmts, int stmt_count) {
    AstStmt *s = malloc(sizeof(AstStmt));
    s->kind = STMT_BLOCK;
    s->data.block.stmts = stmts;
    s->data.block.stmt_count = stmt_count;
    return s;
}

void ast_free_expr(AstExpr *e) {
    if (!e) return;
    
    switch (e->kind) {
        case EXPR_LITERAL:
            free(e->data.literal.value);
            break;
        case EXPR_STRING_LITERAL:
            free(e->data.str_literal.str_value);
            break;
        case EXPR_IDENTIFIER:
            free(e->data.identifier.name);
            break;
        case EXPR_BINARY:
            ast_free_expr(e->data.binary.left);
            ast_free_expr(e->data.binary.right);
            break;
        case EXPR_CALL:
            free(e->data.call.name);
            if (e->data.call.args) {
                for (int i = 0; i < e->data.call.arg_count; i++) {
                    ast_free_expr(e->data.call.args[i]);
                }
                free(e->data.call.args);
            }
            break;
        case EXPR_COMPARISON:
            ast_free_expr(e->data.binary.left);
            ast_free_expr(e->data.binary.right);
            break;
    }
    free(e);
}

void ast_free_stmt(AstStmt *s) {
    if (!s) return;
    
    switch (s->kind) {
        case STMT_EXPR:
            ast_free_expr(s->data.expr);
            break;
        case STMT_ASSIGN:
            free(s->data.assign.name);
            ast_free_expr(s->data.assign.value);
            break;
        case STMT_PRINT:
            if (s->data.print.exprs) {
                for (int i = 0; i < s->data.print.expr_count; i++) {
                    ast_free_expr(s->data.print.exprs[i]);
                }
                free(s->data.print.exprs);
            }
            break;
        case STMT_RETURN:
            ast_free_expr(s->data.ret);
            break;
        case STMT_FUNC_DEF:
            free(s->data.func.name);
            if (s->data.func.params) {
                for (int i = 0; i < s->data.func.param_count; i++) {
                    free(s->data.func.params[i]);
                }
                free(s->data.func.params);
            }
            if (s->data.func.body) {
                for (int i = 0; i < s->data.func.body_count; i++) {
                    ast_free_stmt(s->data.func.body[i]);
                }
                free(s->data.func.body);
            }
            break;
        case STMT_IF:
            ast_free_expr(s->data.if_stmt.cond);
            if (s->data.if_stmt.then_body) {
                for (int i = 0; i < s->data.if_stmt.then_count; i++) {
                    ast_free_stmt(s->data.if_stmt.then_body[i]);
                }
                free(s->data.if_stmt.then_body);
            }
            if (s->data.if_stmt.else_body) {
                for (int i = 0; i < s->data.if_stmt.else_count; i++) {
                    ast_free_stmt(s->data.if_stmt.else_body[i]);
                }
                free(s->data.if_stmt.else_body);
            }
            break;
        case STMT_BLOCK:
            if (s->data.block.stmts) {
                for (int i = 0; i < s->data.block.stmt_count; i++) {
                    ast_free_stmt(s->data.block.stmts[i]);
                }
                free(s->data.block.stmts);
            }
            break;
    }
    free(s);
}
