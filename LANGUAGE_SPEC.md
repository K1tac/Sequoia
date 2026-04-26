# Sequoia Language Specification

Sequoia is a simple yet powerful programming language that compiles to C. It supports variables, functions, printing, and arithmetic operations with proper operator precedence.

## Table of Contents

1. [Basic Syntax](#basic-syntax)
2. [Variables](#variables)
3. [Data Types](#data-types)
4. [Operators](#operators)
5. [Print Statements](#print-statements)
6. [Functions](#functions)
7. [Comments](#comments)
8. [Examples](#examples)

## Basic Syntax

Sequoia programs consist of statements, each ending with a semicolon (`;`).

```
statement1;
statement2;
statement3;
```

## Variables

Variables are declared and assigned in one statement using the `=` operator:

```
x = 10;
y = 20;
z = x + y;
```

Variables are automatically typed as integers. They can hold:
- Integer literals: `42`, `0`, `-15`
- Expressions: `10 + 5 * 2`
- Other variables: `y = x`

### Naming Rules

- Must start with a letter (a-z, A-Z) or underscore (_)
- Can contain letters, digits, and underscores
- Case-sensitive
- Examples: `x`, `count`, `_temp`, `myVariable`

## Data Types

Currently, Sequoia supports:

- **int**: Integer numbers (32-bit signed)
  ```
  value = 42;
  ```

- **string** (limited): Can print strings using `print`
  ```
  print "Hello, World!";
  ```

## Operators

### Arithmetic Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition | `a + b` |
| `-` | Subtraction | `a - b` |
| `*` | Multiplication | `a * b` |
| `/` | Division (integer) | `a / b` |

### Operator Precedence

Sequoia follows standard mathematical precedence:

1. Parentheses: `()`
2. Multiplication and Division: `*`, `/` (left-to-right)
3. Addition and Subtraction: `+`, `-` (left-to-right)

```
2 + 3 * 4;      // Output: 14 (not 20)
(2 + 3) * 4;    // Output: 20
10 / 2 * 3;     // Output: 15 (left-to-right)
```

## Print Statements

The `print` keyword outputs values to the console.

### Print Integer

```
print 42;
print x;
print 10 + 5;
```

### Print String

```
print "Hello, World!";
```

### Print Variables

```
x = 100;
print x;        // Output: 100
print "x is: " x;  // Output: x is: 100
```

## Functions

Functions are reusable blocks of code that can accept parameters and return values.

### Function Definition

```
func add(a, b) {
    result = a + b;
    return result;
}
```

### Function Call

```
result = add(5, 3);
print result;   // Output: 8
```

### Return Statement

Functions use the `return` keyword to send a value back:

```
func square(x) {
    return x * x;
}

y = square(5);
print y;        // Output: 25
```

### Recursive Functions

Sequoia supports recursion:

```
func factorial(n) {
    if n <= 1 {
        return 1;
    }
    return n * factorial(n - 1);
}

print factorial(5);     // Output: 120
```

*Note: Conditionals are supported via `if` statements (see examples).*

## Comments

Single-line comments start with `//`:

```
// This is a comment
x = 10;  // Assign 10 to x
y = 20;  // Assign 20 to y
```

Multi-line comments use `/* ... */`:

```
/*
  This is a multi-line comment.
  It can span multiple lines.
*/
x = 5;
```

## Examples

### Example 1: Simple Arithmetic

```
print "Simple Arithmetic";
a = 10;
b = 5;
print "Sum: " a + b;      // Output: Sum: 15
print "Product: " a * b;   // Output: Product: 50
```

### Example 2: Using Functions

```
func multiply(x, y) {
    return x * y;
}

a = 4;
b = 5;
result = multiply(a, b);
print "Result: " result;   // Output: Result: 20
```

### Example 3: Complex Calculation

```
func calculate_area(width, height) {
    return width * height;
}

print "Area Calculator";
w = 10;
h = 5;
area = calculate_area(w, h);
print "Width: " w;         // Output: Width: 10
print "Height: " h;        // Output: Height: 5
print "Area: " area;       // Output: Area: 50
```

### Example 4: Loop using Recursion

```
func sum_to(n) {
    if n <= 0 {
        return 0;
    }
    return n + sum_to(n - 1);
}

total = sum_to(10);
print "Sum 1 to 10: " total;     // Output: Sum 1 to 10: 55
```

## Best Practices

1. **Use meaningful variable names**: `total_price` instead of `tp`
2. **Add comments for complex logic**: Explain the "why", not the "what"
3. **Break complex calculations into functions**: Makes code reusable
4. **Test with `print` statements**: Use them to debug and verify values
5. **Use parentheses for clarity**: `(a + b) * c` is clearer than `a + b * c`

## Limitations

- Currently supports only integer arithmetic
- No string concatenation (strings are display-only)
- No arrays or complex data structures (planned for future versions)
- Integer division truncates (no floating point yet)
- Maximum function nesting is limited by C's stack

## Future Enhancements

- Floating-point numbers
- String manipulation and concatenation
- Arrays and lists
- More operators (modulo, logical, bitwise)
- Conditional statements (if/else)
- Loop constructs (for, while)
- Error handling
