# Sequoia Language Specification

Sequoia is a small language that compiles to C, builds the generated program with `gcc`, and immediately runs it. The language is centered on integer math, control flow, functions, console I/O, and small interactive programs.

## Program Structure

Sequoia programs are made of statements. Most statements end with a semicolon.

```seq
x = 10;
print x;
```

Block-based constructs such as functions, conditionals, and loops use braces:

```seq
func add(a, b) {
    return a + b;
}

if (1 == 1) {
    print "true";
}

while (0) {
    print "never";
}
```

## Data Model

Sequoia currently supports:

- Integers
- String literals in `print` statements and `input(...)` prompts

Examples:

```seq
42
0
x
"Hello"
```

All runtime values are integers. Strings are not first-class runtime values.

## Variables

Variables are created by assignment.

```seq
score = 0;
name_length = 7;
total = score + name_length;
```

Variables do not need to be declared before use.

## Operators

### Arithmetic

- `+`
- `-`
- `*`
- `/`
- `%`

```seq
result = 2 + 3 * 4;
remainder = 17 % 5;
```

### Unary

- `-expr` negates a value
- `!expr` flips truthiness

```seq
offset = -10;
is_zero = !value;
```

### Comparison

- `<`
- `>`
- `<=`
- `>=`
- `==`
- `!=`

```seq
if (guess == secret) {
    print "correct";
}
```

### Logical

- `&&`
- `||`

Logical operators use integer truthiness and produce integer results.

```seq
if (lives > 0 && score >= target) {
    print "cleared";
}
```

### Precedence

Operator precedence currently works like this:

1. Parentheses
2. Unary operators: `-`, `!`
3. `*`, `/`, `%`
4. `+`, `-`
5. Comparison operators
6. `&&`
7. `||`

```seq
result = !(2 + 3 < 4) || 8 % 3 == 2;
```

## Print

The `print` statement outputs values followed by a newline.

```seq
print 42;
print "Hello";
print "Score: " score;
print "x=" x ", y=" y;
```

Strings and expressions can be mixed in the same `print` statement.

## Functions

Functions are declared with `func`.

```seq
func square(x) {
    return x * x;
}
```

Call them like this:

```seq
result = square(5);
print result;
```

Functions:

- Accept integer parameters
- Return integer values
- Return `0` automatically if execution reaches the end without an explicit `return`

## Return

Use `return` inside a function to send a value back.

```seq
func add(a, b) {
    return a + b;
}
```

You can also return early from a branch:

```seq
func max(a, b) {
    if (a > b) {
        return a;
    }
    return b;
}
```

## Conditionals

Sequoia supports `if`, `else`, and chained `else if` branches.

```seq
if (value < 10) {
    print "small";
} else if (value < 100) {
    print "medium";
} else {
    print "large";
}
```

Conditions use integer truthiness:

- `0` means false
- Any nonzero value means true

## Loops

Sequoia supports `while` loops.

```seq
count = 0;

while (count < 3) {
    print "count = " count;
    count = count + 1;
}
```

### Loop Control

Use `break` to exit the current loop and `continue` to skip to the next iteration.

```seq
count = 0;

while (count < 10) {
    count = count + 1;

    if (count == 5) {
        continue;
    }

    if (count == 8) {
        break;
    }

    print count;
}
```

## Built-in Functions

### `input`

Reads one integer from standard input.

```seq
value = input();
value = input("Enter a number: ");
```

Notes:

- The optional string argument is printed as a prompt
- If input fails, the generated program stores `0`

### `rng`

Produces random integers using C's `rand()`.

```seq
value = rng();
small = rng(10);
between = rng(1, 10);
```

Behavior:

- `rng()` returns a raw random integer
- `rng(max)` returns a value from `0` to `max`, inclusive
- `rng(min, max)` returns a value from `min` to `max`, inclusive
- If `min > max`, Sequoia swaps the bounds

Generated programs seed randomness at startup.

## Comments

Single-line comments:

```seq
// this is a comment
```

Multi-line comments:

```seq
/*
   this is a block comment
*/
```

## Example Program

```seq
func play(secret, attempts) {
    guess = input("Guess a number between 1 and 10: ");

    if (guess == 0) {
        print "Game ended.";
        return 0;
    }

    if (guess == secret) {
        print "You got it in " attempts " tries!";
        return 0;
    }

    if (guess < secret) {
        print "Too low.";
    } else {
        print "Too high.";
    }

    return play(secret, attempts + 1);
}

print "=== Number Guesser ===";
print "I'm thinking of a number between 1 and 10.";
print "Enter 0 to quit.";

secret = rng(1, 10);
game_status = play(secret, 1);
```

## Building and Running

Build the compiler:

```bash
make
```

Run a Sequoia source file:

```bash
./sequoia tests/testprogram.seq
```

Run the guessing game:

```bash
./sequoia tests/guessergame.seq
```

## Current Limits

Sequoia is still intentionally small.

- Integers only
- Strings are not first-class runtime values
- No arrays
- No floating-point support

## Source Layout

- `compiler/` contains the compiler entry point
- `src/frontend/` contains the lexer, parser, and AST code
- `src/backend/` contains optimization and C code generation
- `tests/` contains sample Sequoia programs
