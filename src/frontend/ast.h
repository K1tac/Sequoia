#ifndef AST_H
#define AST_H

typedef enum {
    EXPR_LITERAL,
    EXPR_STRING_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_IDENTIFIER,
    EXPR_CALL,
    EXPR_COMPARISON
} ExprKind;

typedef enum {
    STMT_EXPR,
    STMT_ASSIGN,
    STMT_PRINT,
    STMT_RETURN,
    STMT_FUNC_DEF,
    STMT_IF,
    STMT_WHILE,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_BLOCK
} StmtKind;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_EQ,
    OP_NE,
    OP_AND,
    OP_OR,
    OP_NEG,
    OP_NOT
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
            Operator op;
            AstExpr *operand;
        } unary;
        struct {
            char *name;
        } identifier;
        struct {
            char *name;
            AstExpr **args;
            int arg_count;
        } call;
        struct {
            char *str_value;
        } str_literal;
    } data;
};

typedef struct {
    char *name;
    int param_count;
    char **params;
    AstStmt **body;
    int body_count;
} FuncDef;

typedef struct {
    AstExpr *cond;
    AstStmt **then_body;
    int then_count;
    AstStmt **else_body;
    int else_count;
} IfStmt;

typedef struct {
    AstExpr *cond;
    AstStmt **body;
    int body_count;
} WhileStmt;

struct AstStmt {
    StmtKind kind;
    union {
        AstExpr *expr;
        struct {
            char *name;
            AstExpr *value;
        } assign;
        struct {
            AstExpr **exprs;
            int expr_count;
        } print;
        AstExpr *ret;
        FuncDef func;
        IfStmt if_stmt;
        WhileStmt while_stmt;
        struct {
            AstStmt **stmts;
            int stmt_count;
        } block;
    } data;
};

AstExpr *ast_literal(const char *v);
AstExpr *ast_string_literal(const char *v);
AstExpr *ast_identifier(const char *n);
AstExpr *ast_binary(AstExpr *l, Operator op, AstExpr *r);
AstExpr *ast_unary(Operator op, AstExpr *operand);
AstExpr *ast_call(const char *name, AstExpr **args, int arg_count);

AstStmt *ast_expr_stmt(AstExpr *e);
AstStmt *ast_assign(const char *n, AstExpr *v);
AstStmt *ast_print(AstExpr **exprs, int expr_count);
AstStmt *ast_return(AstExpr *expr);
AstStmt *ast_func_def(const char *name, char **params, int param_count, AstStmt **body, int body_count);
AstStmt *ast_if(AstExpr *cond, AstStmt **then_body, int then_count, AstStmt **else_body, int else_count);
AstStmt *ast_while(AstExpr *cond, AstStmt **body, int body_count);
AstStmt *ast_break(void);
AstStmt *ast_continue(void);
AstStmt *ast_block(AstStmt **stmts, int stmt_count);

void ast_free_expr(AstExpr *e);
void ast_free_stmt(AstStmt *s);

#endif
