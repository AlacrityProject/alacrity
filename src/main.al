// Variable declarations
int x = 5 + 6;
int y = 5 + 5;
int z = 6;

// Basic arithmetic
print(x + y);
print(x - y);
print(x * y);
print(x / y);

int w = x + z;
print(w);

int a = x + y * z;
print(a);

if (x == y) {
     print(x);
}
elif (x == z) {
    print(z);
}
else { 
    print(y);
}

if (x > z) { 
    print(x); 
}
else { 
    print(z); 
}

if (z < x) {
    print(z);
}
else { 
    print(x);
}

if (x != y) {
    print(x); 
}
else { 
    print(y);
}

int p = 6;
if (p == y) { 
    print(y); 
    }
elif (p == z) { 
    print(p); 
}
else { 
    print(x); 
}

print(!x);
print(-x);

// Full expected program evaluation: 21, 1, 110, 1, 17, 71, 10, 11, 6, 11, 6, 0, -11