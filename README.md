# Sequoia

A simple but functional programming language compiler written in C. Sequoia compiles simple arithmetic expressions and statements to C code, which is then compiled and executed.

## Features

- **Lexical Analysis**: Tokenizes input source code
- **Parsing**: Builds an Abstract Syntax Tree (AST) from tokens with proper operator precedence
- **Optimization**: Performs constant folding on arithmetic expressions
- **Code Generation**: Generates C code from the AST
- **Compilation & Execution**: Automatically compiles generated code and runs it

## Language Syntax

### Supported Operations

- **Arithmetic**: `+`, `-`, `*`, `/`
- **Assignment**: `x = expr;`
- **Expressions**: `expr;`

### Examples

```
5 + 3;           // Output: 8
10 * 2;          // Output: 20
2 + 3 * 4;       // Output: 14 (respects operator precedence)
x = 10;          // Assignment
(2 * 3);         // Parenthesized expressions
```

## Building

```bash
make              # Build the compiler
make clean        # Clean build artifacts
make test         # Build and run a simple test
```

## Usage

```bash
./sequoia <filename>
```

Example:
```bash
echo "5 + 3 * 2;" > program.seq
./sequoia program.seq
# Output: Result: 11
```

## Project Structure

```
compiler/
  └── compiler.c        - Main entry point
src/
  ├── frontend/
  │   ├── lexer.c       - Tokenization
  │   ├── parser.c      - Syntax analysis and AST building
  │   └── ast.c         - AST node creation and utilities
  ├── backend/
  │   ├── codegen.c     - C code generation
  │   └── optimizer.c   - Constant folding optimization
  └── runtime/
      ├── math.c        - Mathematical utilities (for future use)
      └── output.c      - Output utilities (for future use)
Makefile
```

## Compilation Pipeline

1. **Lexer** → Tokenizes input (e.g., "2 + 3" → [INT(2), PLUS, INT(3), SEMI])
2. **Parser** → Builds AST with operator precedence
3. **Optimizer** → Applies constant folding transformations
4. **Codegen** → Generates C code
5. **C Compiler** → Compiles generated code to binary
6. **Execution** → Runs the compiled binary and outputs results

## Example Compilation

For input `2 + 3 * 4;`, the generated C code is:

```c
#include <stdio.h>

int main() {
    int result;
    int t0;
    int t1;
    int t2;
    int t3;
    t0 = 2;
    t1 = 3;
    t2 = t1 * 4;
    t3 = t0 + t2;
    result = t3;
    printf("Result: %d\n", result);
    return 0;
}
```

## License

Sequoia is provided under the MIT License.