# Sequoia

Sequoia is a small language that compiles to C, builds the generated program with `gcc`, and immediately runs it. The current language is centered on integer math, function calls, conditionals, console I/O, and small interactive programs.

## Features

- Integer expressions with `+`, `-`, `*`, `/`
- Variables and assignment
- `print` statements with strings and values
- User-defined functions with parameters and `return`
- `if` / `else` conditionals
- Builtin `input(...)` for integer input
- Builtin `rng(...)` for random integers
- Constant folding in the optimizer

## Build

```bash
make
```

## Usage

```bash
./sequoia <file.seq>
```

Example:

```bash
./sequoia tests/testprogram.seq
./sequoia tests/guessergame.seq
```

## Language Snapshot

```seq
func square(x) {
    return x * x;
}

value = rng(1, 10);
guess = input("Your guess: ");

if (guess == value) {
    print "Nice job!";
} else {
    print "The number was " value;
}
```

### Builtins

- `input()` reads an integer.
- `input("Prompt: ")` prints a prompt, then reads an integer.
- `rng()` returns a random integer from C's `rand()`.
- `rng(max)` returns a random integer from `0` to `max`, inclusive.
- `rng(min, max)` returns a random integer from `min` to `max`, inclusive.

If `min > max`, Sequoia swaps them for you.

## Included Examples

- [tests/testprogram.seq](/home/k1tac/Documents/Sequoia/tests/testprogram.seq)
- [tests/input_test.seq](/home/k1tac/Documents/Sequoia/tests/input_test.seq)
- [tests/input_test_v2.seq](/home/k1tac/Documents/Sequoia/tests/input_test_v2.seq)
- [tests/guessergame.seq](/home/k1tac/Documents/Sequoia/tests/guessergame.seq)

## Project Layout

```text
compiler/compiler.c      entry point
src/frontend/            lexer, parser, AST
src/backend/             optimizer and C code generation
tests/                   sample Sequoia programs
```

## Notes

- All runtime values are integers.
- Strings are currently for `print` and `input` prompts.
- There are no loop statements yet, so repeated behavior is typically written with recursion.
