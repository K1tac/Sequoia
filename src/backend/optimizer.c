#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../frontend/ast.h"

static AstExpr *make_int_literal(int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return ast_literal(buf);
}

static AstExpr *optimize_expr(AstExpr *expr) {
    if (!expr) return NULL;
    
    switch (expr->kind) {
        case EXPR_LITERAL:
        case EXPR_STRING_LITERAL:
        case EXPR_IDENTIFIER:
            return expr;
        
        case EXPR_BINARY:
        case EXPR_COMPARISON: {
            AstExpr *left = optimize_expr(expr->data.binary.left);
            AstExpr *right = optimize_expr(expr->data.binary.right);
            
            expr->data.binary.left = left;
            expr->data.binary.right = right;

            if (left && right && left->kind == EXPR_LITERAL && right->kind == EXPR_LITERAL) {
                int lval = atoi(left->data.literal.value);
                int rval = atoi(right->data.literal.value);
                int result = 0;
                
                switch (expr->data.binary.op) {
                    case OP_ADD: result = lval + rval; break;
                    case OP_SUB: result = lval - rval; break;
                    case OP_MUL: result = lval * rval; break;
                    case OP_DIV: result = (rval != 0) ? lval / rval : 0; break;
                    case OP_MOD: result = (rval != 0) ? lval % rval : 0; break;
                    case OP_LT: result = lval < rval; break;
                    case OP_GT: result = lval > rval; break;
                    case OP_LE: result = lval <= rval; break;
                    case OP_GE: result = lval >= rval; break;
                    case OP_EQ: result = lval == rval; break;
                    case OP_NE: result = lval != rval; break;
                    case OP_AND: result = lval && rval; break;
                    case OP_OR: result = lval || rval; break;
                    default: break;
                }

                ast_free_expr(left);
                ast_free_expr(right);
                AstExpr *replacement = make_int_literal(result);
                free(expr);
                return replacement;
            }
            
            return expr;
        }

        case EXPR_UNARY: {
            AstExpr *operand = optimize_expr(expr->data.unary.operand);
            expr->data.unary.operand = operand;

            if (operand && operand->kind == EXPR_LITERAL) {
                int value = atoi(operand->data.literal.value);
                int result = 0;

                switch (expr->data.unary.op) {
                    case OP_NEG: result = -value; break;
                    case OP_NOT: result = !value; break;
                    default: return expr;
                }

                ast_free_expr(operand);
                AstExpr *replacement = make_int_literal(result);
                free(expr);
                return replacement;
            }

            return expr;
        }

        case EXPR_CALL:
            for (int i = 0; i < expr->data.call.arg_count; i++) {
                expr->data.call.args[i] = optimize_expr(expr->data.call.args[i]);
            }
            return expr;
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

        case STMT_PRINT:
            for (int i = 0; i < stmt->data.print.expr_count; i++) {
                stmt->data.print.exprs[i] = optimize_expr(stmt->data.print.exprs[i]);
            }
            return stmt;

        case STMT_RETURN:
            stmt->data.ret = optimize_expr(stmt->data.ret);
            return stmt;

        case STMT_FUNC_DEF:
            for (int i = 0; i < stmt->data.func.body_count; i++) {
                stmt->data.func.body[i] = optimize_stmt(stmt->data.func.body[i]);
            }
            return stmt;

        case STMT_IF:
            stmt->data.if_stmt.cond = optimize_expr(stmt->data.if_stmt.cond);
            for (int i = 0; i < stmt->data.if_stmt.then_count; i++) {
                stmt->data.if_stmt.then_body[i] = optimize_stmt(stmt->data.if_stmt.then_body[i]);
            }
            for (int i = 0; i < stmt->data.if_stmt.else_count; i++) {
                stmt->data.if_stmt.else_body[i] = optimize_stmt(stmt->data.if_stmt.else_body[i]);
            }
            return stmt;

        case STMT_WHILE:
            stmt->data.while_stmt.cond = optimize_expr(stmt->data.while_stmt.cond);
            for (int i = 0; i < stmt->data.while_stmt.body_count; i++) {
                stmt->data.while_stmt.body[i] = optimize_stmt(stmt->data.while_stmt.body[i]);
            }
            return stmt;

        case STMT_BREAK:
        case STMT_CONTINUE:
            return stmt;

        case STMT_BLOCK:
            for (int i = 0; i < stmt->data.block.stmt_count; i++) {
                stmt->data.block.stmts[i] = optimize_stmt(stmt->data.block.stmts[i]);
            }
            return stmt;
    }
    
    return stmt;
}

void optimize(AstStmt *stmt) {
    if (stmt) {
        optimize_stmt(stmt);
    }
}
