# Alacrity

A handwritten programming language made in C

---

## Features

### Variable Declarations
Variables are declared with a type keyword followed by a name and an intitial value. Currently only `int` is supported

```
int x = 5;
int y = x + 3;
```

### Arithmetic
Supports `+`, `-`, `*`, `/` with correct operator precedence: multiplication and division bind tighter than addition and subtraction.

```
int a = x + y * z;   // y * z is evaluated first
print(x + y);
print(x - y);
print(x * y);
print(x / y);        // integer division
```

### Unary Operators
```
print(-x);   // negation
print(!x);   // logical not — returns 0 if x is non-zero, 1 if x is zero
```

### Increment and Decrement
Postfix only. Modifies the variable in place.
```
x++;   // x = x + 1
x--;   // x = x - 1
```

### Comparisons
All comparisons return 1 (true) or 0 (false).
```
x == y   // equal to
x != y   // not equal to
x > y    // greater than
x < y    // less than
x >= y   // greater than or equal to
x <= y   // less than or equal to
```

### Print
Prints an integer expression to stdout.
```
print(x);
print(x + y);
```

### If / Elif / Else
Conditional branching.
```
if (x == y) {
    print(x);
}
elif (x == z) {
    print(z);
}
else {
    print(y);
}
```

### Comments
Single-line comments only, using `//`. 
```
// This is a comment
```

## Example Program

```
int x = 5 + 6;
int y = 5 + 5;
int z = 6;

print(x + y);    // 21
print(x - y);    // 1
print(x * y);    // 110
print(x / y);    // 1

if (x == y) {
    print(x);
}
elif (x == z) {
    print(z);
}
else {
    print(y);    // 10
}

print(!x);       // 0
print(-x);       // -11

int counter = 5;
counter++;
print(counter);  // 6
```

---

## TODO

* Implement more types:
    - Floats
    - Strings
    - Booleans

* Implement loops:
    - For loops