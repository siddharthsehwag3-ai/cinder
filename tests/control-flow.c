int main() {
    int total = 0;

    for (
        int number = 0;
        number < 20;
        number = number + 1
    ) {
        if (number == 4) {
            continue;
        }

        if (number == 12) {
            break;
        }

        total = total + number;
    }

    printf("total = %d\n", total);

    return total;
}