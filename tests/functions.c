int factorial(int number) {
    if (number <= 1) {
        return 1;
    }

    return number * factorial(number - 1);
}

int fibonacci(int number) {
    if (number < 2) {
        return number;
    }

    return fibonacci(number - 1) + fibonacci(number - 2);
}

int main() {
    printf("7! = %d\n", factorial(7));
    printf("fib(10) = %d\n", fibonacci(10));

    return 0;
}