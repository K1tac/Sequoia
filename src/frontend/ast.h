#ifndef AST_H
#define AST_H

typedef enum {
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_IDENTIFIER
} ExprKind;

typedef enum {
    STMT_EXPR,
    STMT_ASSIGN
} StmtKind;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} Operator;

typedef struct AstExpr AstExpr;
typedef struct AstStmt AstStmt;

struct AstExpr {
    ExprKind kind;
    union {
        struct {
            char *value;
        } literal;
        struct {
            AstExpr *left;
            Operator op;
            AstExpr *right;
        } binary;
        struct {
            char *name;
        } identifier;
    } data;
};

struct AstStmt {
    StmtKind kind;
    union {
        AstExpr *expr;
        struct {
            char *name;
            AstExpr *value;
        } assign;
    } data;
};

AstExpr *ast_literal(const char *v);
AstExpr *ast_identifier(const char *n);
AstExpr *ast_binary(AstExpr *l, Operator op, AstExpr *r);
AstStmt *ast_expr_stmt(AstExpr *e);
AstStmt *ast_assign(const char *n, AstExpr *v);
void ast_free_expr(AstExpr *e);
void ast_free_stmt(AstStmt *s);

#endif
