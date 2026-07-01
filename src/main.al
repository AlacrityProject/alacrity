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