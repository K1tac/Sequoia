#include <stdio.h>
#include <stdlib.h>
#include "../src/frontend/lexer.h"
#include "../src/frontend/parser.h"
#include "../src/frontend/ast.h"
#include "../src/backend/codegen.h"
#include "../src/backend/optimizer.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: sequoia <file>\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        printf("error: failed to open file: %s\n", argv[1]);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *src = malloc(size + 1);
    if (!src) {
        printf("error: memory allocation failed\n");
        fclose(f);
        return 1;
    }

    fread(src, 1, size, f);
    src[size] = 0;
    fclose(f);

    Lexer *lexer = lexer_new(src);
    Parser *parser = parser_new(lexer);

    AstStmt *stmt = parse_stmt(parser);

    if (stmt) {
        optimize(stmt);
        generate(stmt);
        ast_free_stmt(stmt);
    }

    parser_free(parser);
    lexer_free(lexer);
    free(src);

    return 0;
}
