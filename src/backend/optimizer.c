#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../frontend/ast.h"

static AstExpr *optimize_expr(AstExpr *expr) {
    if (!expr) return NULL;
    
    switch (expr->kind) {
        case EXPR_LITERAL:
        case EXPR_IDENTIFIER:
            return expr;
        
        case EXPR_BINARY: {
            AstExpr *left = optimize_expr(expr->data.binary.left);
            AstExpr *right = optimize_expr(expr->data.binary.right);
            
            expr->data.binary.left = left;
            expr->data.binary.right = right;
            
            // Only do constant folding if both sides are literals
            if (left && right && left->kind == EXPR_LITERAL && right->kind == EXPR_LITERAL) {
                int lval = atoi(left->data.literal.value);
                int rval = atoi(right->data.literal.value);
                int result = 0;
                
                switch (expr->data.binary.op) {
                    case OP_ADD: result = lval + rval; break;
                    case OP_SUB: result = lval - rval; break;
                    case OP_MUL: result = lval * rval; break;
                    case OP_DIV: result = (rval != 0) ? lval / rval : 0; break;
                }
                
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", result);
                
                // Don't free children - they're already included in expr
                expr->kind = EXPR_LITERAL;
                free(expr->data.binary.left->data.literal.value);
                free(expr->data.binary.left);
                free(expr->data.binary.right->data.literal.value);
                free(expr->data.binary.right);
                expr->data.literal.value = strdup(buf);
            }
            
            return expr;
        }
    }
    
    return expr;
}

static AstStmt *optimize_stmt(AstStmt *stmt) {
    if (!stmt) return NULL;
    
    switch (stmt->kind) {
        case STMT_EXPR:
            stmt->data.expr = optimize_expr(stmt->data.expr);
            return stmt;
        
        case STMT_ASSIGN:
            stmt->data.assign.value = optimize_expr(stmt->data.assign.value);
            return stmt;
    }
    
    return stmt;
}

void optimize(AstStmt *stmt) {
    if (stmt) {
        optimize_stmt(stmt);
    }
}
