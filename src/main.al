func calculate_bonus(int salary, int performance_score) -> int {
    if (performance_score > 80) {
        int base_bonus = 500;
        return salary + base_bonus;
    }
    return salary;
}

int my_salary = 5000;
int my_score = 95;
int total_pay = calculate_bonus(my_salary, my_score);
print(total_pay); // Expected: 5500

int x = 0;

string my_string = "Hello World";

while (x < 10) {
    print(x);
    x++;
}

print(my_string);