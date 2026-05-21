# Alacrity

A handwritten programming language made in C

---

## Features

### Variable Declarations
Variables are declared with a type keyword followed by a name and an initial value. 

Below are the supported types:
```
int integer_example = 5;
float float_example = 3.14;
string string_example = "Hello World!";
bool bool_example = true; 
```

### Variable Reassignment
Variable values can be reassigned after they have been declared.

```
int x = 5;

x = 5 + 10;
```

### Arithmetic
Supports `+`, `-`, `*`, `/` with correct operator precedence: multiplication and division bind tighter than addition and subtraction.

Integer and float mixed operations are promoted to floats

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
print(!x);   // logical not — returns false if x is non-zero, true if x is zero
```

### Increment and Decrement
Postfix only. Modifies the variable in place.
```
x++;   // x = x + 1
x--;   // x = x - 1
```

### Comparisons
All comparisons return true or false.
```
x == y   // equal to
x != y   // not equal to
x > y    // greater than
x < y    // less than
x >= y   // greater than or equal to
x <= y   // less than or equal to
```

### Print
Prints an expression to stdout.
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

### While Loops
While loops that run based on a single expression are supported.

```
int x = 0;

while (x < 10) {
    print(x);
    x++;
}
```

### Functions
Functions are supported in Alacrity and are declared using `func` and the return type arrow after the name and parameter declaration.

```
func add(int x, int y) -> int {
    return x + y;
}

int result = add(5, 10);
```

### Example Program 
A full example program showcasing all implemented functionality

```
// Variable declarations
int x = 10;
int y = 3;
float pi = 3.14;
string greeting = "Hello World";
bool flag = true;

// Print statements
print(greeting);
print(x + y);
print(x * y);
print(pi);

// Comparisons and if/elif/else
if (x > y) {
    print(x);
}
elif (x == y) {
    print(0);
}
else {
    print(y);
}

// While loop with increment
int counter = 0;
while (counter < 5) {
    print(counter);
    counter++;
}

// Functions
func multiply(int a, int b) -> int {
    return a * b;
}

int result = multiply(x, y);
print(result);

// Variable reassignment
x = x + 100;
print(x);
```
---

## TODO

* Type safety:
    - assigning a bool to an int variable works silently

* Compound assignment operators:
    - +=, -= etc.

* String operations:
    - concatenation, length, etc.

* Better error messages with line numbers

* Implement loops:
    - For loops

* Implement arrays/lists