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

void ast_free_expr(AstExpr *e) {
    if (!e) return;
    
    switch (e->kind) {
        case EXPR_LITERAL:
            free(e->data.literal.value);
            break;
        case EXPR_IDENTIFIER:
            free(e->data.identifier.name);
            break;
        case EXPR_BINARY:
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
    }
    free(s);
}
