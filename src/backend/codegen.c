#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../frontend/ast.h"

static int temp_counter = 0;

static void gen_expr(AstExpr *expr, FILE *out, const char *dest_var);

static void gen_expr(AstExpr *expr, FILE *out, const char *dest_var) {
    if (!expr) return;
    
    switch (expr->kind) {
        case EXPR_LITERAL:
            fprintf(out, "    %s = %s;\n", dest_var, expr->data.literal.value);
            break;
        
        case EXPR_IDENTIFIER:
            fprintf(out, "    %s = %s;\n", dest_var, expr->data.identifier.name);
            break;
        
        case EXPR_BINARY: {
            char left_var[32], right_var[32];
            snprintf(left_var, sizeof(left_var), "t%d", temp_counter++);
            snprintf(right_var, sizeof(right_var), "t%d", temp_counter++);
            
            const char *op_str = "";
            switch (expr->data.binary.op) {
                case OP_ADD: op_str = "+"; break;
                case OP_SUB: op_str = "-"; break;
                case OP_MUL: op_str = "*"; break;
                case OP_DIV: op_str = "/"; break;
            }
            
            fprintf(out, "    int %s;\n", left_var);
            fprintf(out, "    int %s;\n", right_var);
            gen_expr(expr->data.binary.left, out, left_var);
            gen_expr(expr->data.binary.right, out, right_var);
            fprintf(out, "    %s = %s %s %s;\n", dest_var, left_var, op_str, right_var);
            break;
        }
    }
}

void generate(AstStmt *stmt) {
    if (!stmt) return;
    
    FILE *out = fopen("__sequoia_out.c", "w");
    if (!out) {
        printf("error: could not open output file\n");
        return;
    }
    
    fprintf(out, "#include <stdio.h>\n\n");
    fprintf(out, "int main() {\n");
    
    temp_counter = 0;
    
    switch (stmt->kind) {
        case STMT_EXPR: {
            fprintf(out, "    int result;\n");
            gen_expr(stmt->data.expr, out, "result");
            fprintf(out, "    printf(\"Result: %%d\\n\", result);\n");
            break;
        }
        
        case STMT_ASSIGN: {
            fprintf(out, "    int %s;\n", stmt->data.assign.name);
            gen_expr(stmt->data.assign.value, out, stmt->data.assign.name);
            fprintf(out, "    printf(\"%%s = %%d\\n\", \"%s\", %s);\n", stmt->data.assign.name, stmt->data.assign.name);
            break;
        }
    }
    
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
    
    fclose(out);
    
    int ret = system("gcc -o __sequoia_bin __sequoia_out.c 2>/dev/null");
    if (ret == 0) {
        printf("Compiled successfully. Running...\n");
        system("./__sequoia_bin");
    } else {
        printf("Compilation failed. Generated code saved to __sequoia_out.c\n");
    }
}
