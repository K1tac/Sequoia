#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../frontend/ast.h"
#include "../frontend/parser.h"

typedef struct {
    char **names;
    int count;
    int capacity;
} Scope;

static int temp_counter = 0;

static void scope_init(Scope *scope) {
    scope->names = NULL;
    scope->count = 0;
    scope->capacity = 0;
}

static void scope_free(Scope *scope) {
    if (!scope) {
        return;
    }

    for (int i = 0; i < scope->count; i++) {
        free(scope->names[i]);
    }

    free(scope->names);
}

static int scope_contains(const Scope *scope, const char *name) {
    for (int i = 0; i < scope->count; i++) {
        if (strcmp(scope->names[i], name) == 0) {
            return 1;
        }
    }

    return 0;
}

static void scope_add(Scope *scope, const char *name) {
    if (scope_contains(scope, name)) {
        return;
    }

    if (scope->count == scope->capacity) {
        int new_capacity = scope->capacity == 0 ? 16 : scope->capacity * 2;
        scope->names = realloc(scope->names, sizeof(char *) * new_capacity);
        scope->capacity = new_capacity;
    }

    scope->names[scope->count++] = strdup(name);
}

static void emit_indent(FILE *out, int indent) {
    for (int i = 0; i < indent; i++) {
        fputs("    ", out);
    }
}

static void emit_decl(FILE *out, Scope *scope, const char *name, int indent) {
    if (!scope) {
        return;
    }

    if (scope_contains(scope, name)) {
        return;
    }

    scope_add(scope, name);
    emit_indent(out, indent);
    fprintf(out, "int %s;\n", name);
}

static void escape_c_string(FILE *out, const char *value) {
    for (const unsigned char *p = (const unsigned char *)value; *p; p++) {
        switch (*p) {
            case '\\':
                fputs("\\\\", out);
                break;
            case '"':
                fputs("\\\"", out);
                break;
            case '\n':
                fputs("\\n", out);
                break;
            case '\r':
                fputs("\\r", out);
                break;
            case '\t':
                fputs("\\t", out);
                break;
            default:
                fputc(*p, out);
                break;
        }
    }
}

static const char *op_to_string(Operator op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_LT: return "<";
        case OP_GT: return ">";
        case OP_LE: return "<=";
        case OP_GE: return ">=";
        case OP_EQ: return "==";
        case OP_NE: return "!=";
        default: return "+";
    }
}

static void next_temp(char *buf, size_t size) {
    snprintf(buf, size, "t%d", temp_counter++);
}

static void gen_expr(AstExpr *expr, FILE *out, Scope *scope, const char *dest_var, int indent);

static void gen_call(AstExpr *expr, FILE *out, Scope *scope, const char *dest_var, int indent) {
    char args[1024] = {0};

    if (strcmp(expr->data.call.name, "input") == 0) {
        if (expr->data.call.arg_count > 1) {
            emit_indent(out, indent);
            fprintf(out, "%s = 0;\n", dest_var);
            return;
        }

        if (expr->data.call.arg_count == 1) {
            AstExpr *prompt = expr->data.call.args[0];

            if (prompt->kind != EXPR_STRING_LITERAL) {
                emit_indent(out, indent);
                fprintf(out, "%s = 0;\n", dest_var);
                return;
            }

            emit_indent(out, indent);
            fputs("printf(\"", out);
            escape_c_string(out, prompt->data.str_literal.str_value);
            fputs("\");\n", out);

            emit_indent(out, indent);
            fputs("fflush(stdout);\n", out);
        }

        emit_indent(out, indent);
        fprintf(out, "if (scanf(\"%%d\", &%s) != 1) {\n", dest_var);
        emit_indent(out, indent + 1);
        fprintf(out, "%s = 0;\n", dest_var);
        emit_indent(out, indent);
        fputs("}\n", out);
        return;
    }

    for (int i = 0; i < expr->data.call.arg_count; i++) {
        char arg_var[32];
        next_temp(arg_var, sizeof(arg_var));
        emit_decl(out, scope, arg_var, indent);
        gen_expr(expr->data.call.args[i], out, scope, arg_var, indent);

        if (i > 0) {
            strncat(args, ", ", sizeof(args) - strlen(args) - 1);
        }
        strncat(args, arg_var, sizeof(args) - strlen(args) - 1);
    }

    emit_indent(out, indent);
    fprintf(out, "%s = %s(%s);\n", dest_var, expr->data.call.name, args);
}

static void gen_expr(AstExpr *expr, FILE *out, Scope *scope, const char *dest_var, int indent) {
    if (!expr) {
        return;
    }

    switch (expr->kind) {
        case EXPR_LITERAL:
            emit_indent(out, indent);
            fprintf(out, "%s = %s;\n", dest_var, expr->data.literal.value);
            break;

        case EXPR_STRING_LITERAL:
            emit_indent(out, indent);
            fprintf(out, "%s = 0;\n", dest_var);
            break;

        case EXPR_IDENTIFIER:
            emit_indent(out, indent);
            fprintf(out, "%s = %s;\n", dest_var, expr->data.identifier.name);
            break;

        case EXPR_BINARY:
        case EXPR_COMPARISON: {
            char left_var[32];
            char right_var[32];

            next_temp(left_var, sizeof(left_var));
            next_temp(right_var, sizeof(right_var));

            emit_decl(out, scope, left_var, indent);
            emit_decl(out, scope, right_var, indent);

            gen_expr(expr->data.binary.left, out, scope, left_var, indent);
            gen_expr(expr->data.binary.right, out, scope, right_var, indent);

            emit_indent(out, indent);
            fprintf(out, "%s = %s %s %s;\n",
                    dest_var,
                    left_var,
                    op_to_string(expr->data.binary.op),
                    right_var);
            break;
        }

        case EXPR_CALL:
            gen_call(expr, out, scope, dest_var, indent);
            break;
    }
}

static void gen_stmt(AstStmt *stmt, FILE *out, Scope *scope, int indent) {
    if (!stmt) {
        return;
    }

    switch (stmt->kind) {
        case STMT_EXPR: {
            char result_var[32];
            next_temp(result_var, sizeof(result_var));
            emit_decl(out, scope, result_var, indent);
            gen_expr(stmt->data.expr, out, scope, result_var, indent);
            emit_indent(out, indent);
            fprintf(out, "printf(\"Result: %%d\\n\", %s);\n", result_var);
            break;
        }

        case STMT_ASSIGN:
            emit_decl(out, scope, stmt->data.assign.name, indent);
            gen_expr(stmt->data.assign.value, out, scope, stmt->data.assign.name, indent);
            break;

        case STMT_PRINT: {
            char **value_names = NULL;

            if (stmt->data.print.expr_count > 0) {
                value_names = calloc((size_t)stmt->data.print.expr_count, sizeof(char *));
            }

            for (int i = 0; i < stmt->data.print.expr_count; i++) {
                AstExpr *expr = stmt->data.print.exprs[i];
                if (expr->kind == EXPR_STRING_LITERAL) {
                    continue;
                }

                char temp_var[32];
                next_temp(temp_var, sizeof(temp_var));
                value_names[i] = strdup(temp_var);
                emit_decl(out, scope, temp_var, indent);
                gen_expr(expr, out, scope, temp_var, indent);
            }

            emit_indent(out, indent);
            fputs("printf(\"", out);

            for (int i = 0; i < stmt->data.print.expr_count; i++) {
                AstExpr *expr = stmt->data.print.exprs[i];
                if (expr->kind == EXPR_STRING_LITERAL) {
                    fputs("%s", out);
                } else {
                    fputs("%d", out);
                }
            }

            fputs("\\n\"", out);

            for (int i = 0; i < stmt->data.print.expr_count; i++) {
                AstExpr *expr = stmt->data.print.exprs[i];

                if (expr->kind == EXPR_STRING_LITERAL) {
                    fputs(", \"", out);
                    escape_c_string(out, expr->data.str_literal.str_value);
                    fputc('"', out);
                } else {
                    fprintf(out, ", %s", value_names[i]);
                }
            }

            fputs(");\n", out);

            for (int i = 0; i < stmt->data.print.expr_count; i++) {
                free(value_names ? value_names[i] : NULL);
            }
            free(value_names);
            break;
        }

        case STMT_RETURN:
            if (stmt->data.ret) {
                char ret_var[32];
                next_temp(ret_var, sizeof(ret_var));
                emit_decl(out, scope, ret_var, indent);
                gen_expr(stmt->data.ret, out, scope, ret_var, indent);
                emit_indent(out, indent);
                fprintf(out, "return %s;\n", ret_var);
            } else {
                emit_indent(out, indent);
                fputs("return 0;\n", out);
            }
            break;

        case STMT_FUNC_DEF: {
            Scope func_scope;
            scope_init(&func_scope);

            fprintf(out, "int %s(", stmt->data.func.name);
            for (int i = 0; i < stmt->data.func.param_count; i++) {
                if (i > 0) {
                    fputs(", ", out);
                }
                fprintf(out, "int %s", stmt->data.func.params[i]);
                scope_add(&func_scope, stmt->data.func.params[i]);
            }
            fputs(") {\n", out);

            for (int i = 0; i < stmt->data.func.body_count; i++) {
                gen_stmt(stmt->data.func.body[i], out, &func_scope, 1);
            }

            emit_indent(out, 1);
            fputs("return 0;\n", out);
            fputs("}\n\n", out);
            scope_free(&func_scope);
            break;
        }

        case STMT_IF: {
            char cond_var[32];
            next_temp(cond_var, sizeof(cond_var));
            emit_decl(out, scope, cond_var, indent);
            gen_expr(stmt->data.if_stmt.cond, out, scope, cond_var, indent);

            emit_indent(out, indent);
            fprintf(out, "if (%s) {\n", cond_var);
            for (int i = 0; i < stmt->data.if_stmt.then_count; i++) {
                gen_stmt(stmt->data.if_stmt.then_body[i], out, scope, indent + 1);
            }
            emit_indent(out, indent);
            fputs("}", out);

            if (stmt->data.if_stmt.else_body && stmt->data.if_stmt.else_count > 0) {
                fputs(" else {\n", out);
                for (int i = 0; i < stmt->data.if_stmt.else_count; i++) {
                    gen_stmt(stmt->data.if_stmt.else_body[i], out, scope, indent + 1);
                }
                emit_indent(out, indent);
                fputs("}\n", out);
            } else {
                fputc('\n', out);
            }
            break;
        }

        case STMT_BLOCK:
            for (int i = 0; i < stmt->data.block.stmt_count; i++) {
                gen_stmt(stmt->data.block.stmts[i], out, scope, indent);
            }
            break;
    }
}

void generate(Program *prog) {
    if (!prog) {
        return;
    }

    FILE *out = fopen("__sequoia_out.c", "w");
    if (!out) {
        printf("error: could not open output file\n");
        return;
    }

    fprintf(out, "#include <stdio.h>\n\n");

    temp_counter = 0;

    for (int i = 0; i < prog->count; i++) {
        if (prog->stmts[i]->kind == STMT_FUNC_DEF) {
            gen_stmt(prog->stmts[i], out, NULL, 0);
        }
    }

    fputs("int main() {\n", out);

    Scope main_scope;
    scope_init(&main_scope);

    for (int i = 0; i < prog->count; i++) {
        if (prog->stmts[i]->kind != STMT_FUNC_DEF) {
            gen_stmt(prog->stmts[i], out, &main_scope, 1);
        }
    }

    emit_indent(out, 1);
    fputs("return 0;\n", out);
    fputs("}\n", out);
    fclose(out);

    scope_free(&main_scope);

    int ret = system("gcc -o __sequoia_bin __sequoia_out.c 2>/dev/null");
    if (ret == 0) {
        printf("Compiled successfully. Running...\n");
        fflush(stdout);
        system("./__sequoia_bin");
    } else {
        printf("Compilation failed. Generated code saved to __sequoia_out.c\n");
    }
}
