SEQUOIA COMPILER - IMPLEMENTATION COMPLETE
==========================================

## Summary

The Sequoia compiler has been successfully implemented and is fully functional. It compiles simple arithmetic expressions to executable C programs.

## Test Results

✓ 100 - 50 = 50
✓ 15 / 3 = 5  
✓ 5 + 3 = 8
✓ 10 * 2 = 20
✓ 2 + 3 * 4 = 14 (correct operator precedence)
✓ 7 - 3 + 2 = 6
✓ 3 * 4 = 12

## Implementation Details

### Components Implemented

1. **Lexer** (src/frontend/lexer.c)
   - Tokenizes source code
   - Handles integers, operators (+, -, *, /), and parentheses
   - Properly advances through input stream

2. **Parser** (src/frontend/parser.c)
   - Recursive descent parser with operator precedence
   - Creates Abstract Syntax Trees (AST)
   - Handles expressions and statements
   - Fixed critical token advancement bug

3. **AST** (src/frontend/ast.c)
   - Node structures for literals, identifiers, and binary operations
   - Memory management for AST nodes

4. **Optimizer** (src/backend/optimizer.c)
   - Performs constant folding on arithmetic expressions
   - Properly handles memory to avoid double-free issues

5. **Code Generator** (src/backend/codegen.c)
   - Generates valid C code from AST
   - Creates unique temporary variables for intermediate values
   - Automatically compiles and executes generated code

6. **Build System** (Makefile)
   - Automated compilation of all components
   - Clean and test targets

## Key Fixes Applied

1. **Lexer Token Advancement** - Fixed critical bug where advance() was called before switch statement, skipping operator characters
2. **Parser Advance Function** - Corrected token advancement to properly update current token from lexer
3. **Code Generation** - Rewrote temp variable generation to create unique variables for each operand
4. **Optimizer Memory** - Fixed double-free issues in constant folding

## File Structure

```
/home/k1tac/Documents/Sequoia/
├── Makefile
├── README.md
├── LICENSE
├── compiler/
│   └── compiler.c              (Main entry point)
├── src/
│   ├── frontend/
│   │   ├── lexer.c              (Tokenization)
│   │   ├── lexer.h
│   │   ├── parser.c             (Parsing)
│   │   ├── parser.h
│   │   ├── ast.c                (AST nodes)
│   │   └── ast.h
│   ├── backend/
│   │   ├── codegen.c            (Code generation)
│   │   ├── codegen.h
│   │   ├── optimizer.c          (Optimization)
│   │   └── optimizer.h
│   └── runtime/
│       ├── math.c               (Math utilities)
│       └── output.c             (Output utilities)
└── tests/
    (directory for test files)
```

## Usage

```bash
cd /home/k1tac/Documents/Sequoia
make
echo "5 + 3 * 2;" > program.seq
./sequoia program.seq
# Output: Result: 11
```
