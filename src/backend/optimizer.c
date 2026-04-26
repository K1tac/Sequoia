#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../frontend/ast.h"

static int expr_is_int_literal(const AstExpr *expr, int *value) {
    if (!expr || expr->kind != EXPR_LITERAL) {
        return 0;
    }

    if (value) {
        *value = atoi(expr->data.literal.value);
    }

    return 1;
}

static AstExpr *make_int_literal(int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return ast_literal(buf);
}

static AstStmt *make_empty_stmt(void) {
    return ast_expr_stmt(ast_literal("0"));
}

static int stmt_always_exits(const AstStmt *stmt) {
    if (!stmt) {
        return 0;
    }

    switch (stmt->kind) {
        case STMT_RETURN:
        case STMT_BREAK:
        case STMT_CONTINUE:
            return 1;

        case STMT_IF:
            if (stmt->data.if_stmt.then_count == 0 || stmt->data.if_stmt.else_count == 0) {
                return 0;
            }
            return stmt_always_exits(stmt->data.if_stmt.then_body[stmt->data.if_stmt.then_count - 1]) &&
                   stmt_always_exits(stmt->data.if_stmt.else_body[stmt->data.if_stmt.else_count - 1]);

        case STMT_BLOCK:
            if (stmt->data.block.stmt_count == 0) {
                return 0;
            }
            return stmt_always_exits(stmt->data.block.stmts[stmt->data.block.stmt_count - 1]);

        default:
            return 0;
    }
}

static int prune_stmt_list(AstStmt **stmts, int count) {
    int kept = 0;

    for (int i = 0; i < count; i++) {
        if (kept < i) {
            ast_free_stmt(stmts[i]);
            continue;
        }

        stmts[kept++] = stmts[i];

        if (stmt_always_exits(stmts[i])) {
            for (int j = i + 1; j < count; j++) {
                ast_free_stmt(stmts[j]);
            }
            break;
        }
    }

    return kept;
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

            int lval = 0;
            int rval = 0;
            int left_is_literal = expr_is_int_literal(left, &lval);
            int right_is_literal = expr_is_int_literal(right, &rval);

            if (left_is_literal && right_is_literal) {
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

            switch (expr->data.binary.op) {
                case OP_ADD:
                    if (left_is_literal && lval == 0) {
                        ast_free_expr(left);
                        free(expr);
                        return right;
                    }
                    if (right_is_literal && rval == 0) {
                        ast_free_expr(right);
                        free(expr);
                        return left;
                    }
                    break;

                case OP_SUB:
                    if (right_is_literal && rval == 0) {
                        ast_free_expr(right);
                        free(expr);
                        return left;
                    }
                    if (left_is_literal && lval == 0) {
                        expr->kind = EXPR_UNARY;
                        expr->data.unary.op = OP_NEG;
                        expr->data.unary.operand = right;
                        ast_free_expr(left);
                        return optimize_expr(expr);
                    }
                    break;

                case OP_MUL:
                    if ((left_is_literal && lval == 0) || (right_is_literal && rval == 0)) {
                        ast_free_expr(left);
                        ast_free_expr(right);
                        AstExpr *replacement = make_int_literal(0);
                        free(expr);
                        return replacement;
                    }
                    if (left_is_literal && lval == 1) {
                        ast_free_expr(left);
                        free(expr);
                        return right;
                    }
                    if (right_is_literal && rval == 1) {
                        ast_free_expr(right);
                        free(expr);
                        return left;
                    }
                    break;

                case OP_DIV:
                    if (left_is_literal && lval == 0) {
                        ast_free_expr(left);
                        ast_free_expr(right);
                        AstExpr *replacement = make_int_literal(0);
                        free(expr);
                        return replacement;
                    }
                    if (right_is_literal && rval == 1) {
                        ast_free_expr(right);
                        free(expr);
                        return left;
                    }
                    break;

                case OP_MOD:
                    if (left_is_literal && lval == 0) {
                        ast_free_expr(left);
                        ast_free_expr(right);
                        AstExpr *replacement = make_int_literal(0);
                        free(expr);
                        return replacement;
                    }
                    if (right_is_literal && rval == 1) {
                        ast_free_expr(left);
                        ast_free_expr(right);
                        AstExpr *replacement = make_int_literal(0);
                        free(expr);
                        return replacement;
                    }
                    break;

                case OP_AND:
                    if ((left_is_literal && lval == 0) || (right_is_literal && rval == 0)) {
                        ast_free_expr(left);
                        ast_free_expr(right);
                        AstExpr *replacement = make_int_literal(0);
                        free(expr);
                        return replacement;
                    }
                    if (left_is_literal && lval != 0) {
                        ast_free_expr(left);
                        free(expr);
                        return right;
                    }
                    if (right_is_literal && rval != 0) {
                        ast_free_expr(right);
                        free(expr);
                        return left;
                    }
                    break;

                case OP_OR:
                    if ((left_is_literal && lval != 0) || (right_is_literal && rval != 0)) {
                        ast_free_expr(left);
                        ast_free_expr(right);
                        AstExpr *replacement = make_int_literal(1);
                        free(expr);
                        return replacement;
                    }
                    if (left_is_literal && lval == 0) {
                        ast_free_expr(left);
                        free(expr);
                        return right;
                    }
                    if (right_is_literal && rval == 0) {
                        ast_free_expr(right);
                        free(expr);
                        return left;
                    }
                    break;

                default:
                    break;
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
            stmt->data.func.body_count = prune_stmt_list(stmt->data.func.body, stmt->data.func.body_count);
            return stmt;

        case STMT_IF: {
            stmt->data.if_stmt.cond = optimize_expr(stmt->data.if_stmt.cond);
            for (int i = 0; i < stmt->data.if_stmt.then_count; i++) {
                stmt->data.if_stmt.then_body[i] = optimize_stmt(stmt->data.if_stmt.then_body[i]);
            }
            for (int i = 0; i < stmt->data.if_stmt.else_count; i++) {
                stmt->data.if_stmt.else_body[i] = optimize_stmt(stmt->data.if_stmt.else_body[i]);
            }
            stmt->data.if_stmt.then_count = prune_stmt_list(stmt->data.if_stmt.then_body, stmt->data.if_stmt.then_count);
            stmt->data.if_stmt.else_count = prune_stmt_list(stmt->data.if_stmt.else_body, stmt->data.if_stmt.else_count);

            int cond_value = 0;
            if (expr_is_int_literal(stmt->data.if_stmt.cond, &cond_value)) {
                AstStmt **selected_body = cond_value ? stmt->data.if_stmt.then_body : stmt->data.if_stmt.else_body;
                int selected_count = cond_value ? stmt->data.if_stmt.then_count : stmt->data.if_stmt.else_count;
                AstStmt *replacement = make_empty_stmt();

                if (selected_count > 0) {
                    replacement = ast_block(selected_body, selected_count);
                    if (cond_value) {
                        stmt->data.if_stmt.else_body = NULL;
                        stmt->data.if_stmt.else_count = 0;
                    } else {
                        stmt->data.if_stmt.then_body = NULL;
                        stmt->data.if_stmt.then_count = 0;
                    }
                }

                ast_free_stmt(stmt);
                return optimize_stmt(replacement);
            }

            return stmt;
        }

        case STMT_WHILE:
            stmt->data.while_stmt.cond = optimize_expr(stmt->data.while_stmt.cond);
            for (int i = 0; i < stmt->data.while_stmt.body_count; i++) {
                stmt->data.while_stmt.body[i] = optimize_stmt(stmt->data.while_stmt.body[i]);
            }
            stmt->data.while_stmt.body_count = prune_stmt_list(stmt->data.while_stmt.body, stmt->data.while_stmt.body_count);

            int cond_value = 0;
            if (expr_is_int_literal(stmt->data.while_stmt.cond, &cond_value) && cond_value == 0) {
                ast_free_stmt(stmt);
                return make_empty_stmt();
            }
            return stmt;

        case STMT_BREAK:
        case STMT_CONTINUE:
            return stmt;

        case STMT_BLOCK:
            for (int i = 0; i < stmt->data.block.stmt_count; i++) {
                stmt->data.block.stmts[i] = optimize_stmt(stmt->data.block.stmts[i]);
            }
            stmt->data.block.stmt_count = prune_stmt_list(stmt->data.block.stmts, stmt->data.block.stmt_count);
            return stmt;
    }
    
    return stmt;
}

void optimize(AstStmt *stmt) {
    if (stmt) {
        optimize_stmt(stmt);
    }
}
