int main(void)
{
    char letter = 'A';

    int value = 5;
    int bits;
    int index = 0;
    int total = 0;
    int chosen;

    value += 3;
    ++value;
    value--;

    bits =
        (5 & 3) |
        (8 >> 2);

    do {
        total += index;
        index++;
    } while (index < 5);

    chosen =
        total == 10
            ? value
            : 0;

    printf(
        "char=%c value=%d bits=%d total=%d\n",
        letter,
        value,
        bits,
        total
    );

    printf(
        "formats=%u %x %X %o\n",
        255,
        255,
        255,
        255
    );

    return chosen;
}