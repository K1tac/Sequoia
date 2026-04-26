# Sequoia Language Specification

Sequoia is an integer-based language that compiles to C and runs the generated program.

## Statements

Every statement ends with `;` unless it is a block-based construct such as `func` or `if`.

```seq
x = 10;
print x;
value = add(x, 2);
```

## Values and Expressions

Sequoia currently works with integers plus string literals for output and prompts.

```seq
42
x
(a + b) * 3
square(5)
"hello"
```

### Operators

- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`

Arithmetic follows standard precedence:

1. Parentheses
2. `*` and `/`
3. `+` and `-`
4. Comparison operators

## Variables

Variables are introduced by assignment.

```seq
score = 0;
target = 7;
score = score + 1;
```

## Print

`print` can output strings, integers, and mixed sequences.

```seq
print "Hello";
print 42;
print "Score: " score;
print "A=" a ", B=" b;
```

Each `print` ends with a newline.

## Functions

Functions accept integer parameters and return integers.

```seq
func add(a, b) {
    return a + b;
}

result = add(2, 3);
```

If execution reaches the end of a function without an explicit `return`, Sequoia emits `return 0;`.

## Conditionals

Sequoia supports `if` and `else`.

```seq
if (guess < target) {
    print "Too low";
} else {
    print "Too high or correct";
}
```

Conditions use integer truthiness: `0` is false, nonzero is true.

## Builtins

### `input`

Reads one integer from standard input.

```seq
value = input();
value = input("Enter a number: ");
```

If reading fails, the generated program stores `0`.

### `rng`

Produces random integers using C's `rand()`. Generated programs seed randomness at startup with the current time.

```seq
any_value = rng();
small_value = rng(10);
between = rng(1, 10);
```

- `rng()` returns a raw random integer.
- `rng(max)` returns a value from `0` to `max`, inclusive.
- `rng(min, max)` returns a value from `min` to `max`, inclusive.
- If `min > max`, Sequoia swaps the bounds.

## Comments

Single-line comments:

```seq
// comment
```

Multi-line comments:

```seq
/* comment
   comment */
```

## Example: Number Guesser

```seq
func play(secret, attempts) {
    guess = input("Guess a number between 1 and 10: ");

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

secret = rng(1, 10);
game_status = play(secret, 1);
```

## Current Limits

- Integers only
- Strings are not first-class values
- No loops yet
- No arrays or floating-point values
