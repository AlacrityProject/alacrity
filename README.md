# Alacrity

A handwritten programming language made in C


## TODO

1. Implement more comparisons into the tokenize, isOperator and getTokenPrecedence functions
    - `<`
    - `>`
    - `<=`
    - `>=`
    - `!=`
2. Implement Unary operators
    - `!` 
    - `-`

3. Implement increment and decrement:
    - `++` shorthand for `variable = variable + 1`
    - `--` shorthand for `variable = variable - 1`
    - postfix only `variable++`

4. Implement functions:
    - Function declaration parsing
    - Storing function definitions (separate from variable storage table)
    - Argument passing
    - Scope for local variables
    - Return mechanism