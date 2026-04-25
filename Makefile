CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

# Source files
LEXER = src/frontend/lexer.c
PARSER = src/frontend/parser.c
AST = src/frontend/ast.c
CODEGEN = src/backend/codegen.c
OPTIMIZER = src/backend/optimizer.c
MAIN = compiler/compiler.c

# Object files
OBJS = $(LEXER:.c=.o) $(PARSER:.c=.o) $(AST:.c=.o) $(CODEGEN:.c=.o) $(OPTIMIZER:.c=.o) $(MAIN:.c=.o)

# Executable
BINARY = sequoia

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(BINARY) __sequoia_out.c __sequoia_bin

test: $(BINARY)
	echo "5 + 3;" > test.seq
	./$(BINARY) test.seq
	rm -f test.seq

.PHONY: all clean test
