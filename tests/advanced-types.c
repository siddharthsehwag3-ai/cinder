enum Status {
    STATUS_IDLE = 2,
    STATUS_RUNNING,
    STATUS_DONE = 8
};

struct Point {
    int x;
    int y;
};

struct Record {
    struct Point position;
    unsigned short code;
    char grade;
};

int classify(
    int value
)
{
    switch (value) {
        case 0:
            return 100;

        case 1:
        case 2:
            return 200;

        case 3:
            return 300;

        default:
            return -1;
    }
}

int point_total(
    struct Point *point
)
{
    return
        point->x +
        point->y;
}

int main()
{
    const int fixed = 7;

    struct Point point = {
        12,
        30
    };

    struct Record record = {
        {
            4,
            9
        },
        65530,
        'A'
    };

    enum Status status =
        STATUS_RUNNING;

    unsigned char small = 250;
    signed char negative = -12;
    unsigned short medium = 65000;
    long large = 100000;
    unsigned long flags = 15u;

    int result =
        point_total(
            &point
        );

    result +=
        classify(2);

    result +=
        record.position.x;

    result +=
        record.position.y;

    printf(
        "point=%d,%d total=%d\n",
        point.x,
        point.y,
        point_total(&point)
    );

    printf(
        "record=%d,%d code=%u grade=%c\n",
        record.position.x,
        record.position.y,
        record.code,
        record.grade
    );

    printf(
        "enum=%d switch=%d const=%d\n",
        status,
        classify(2),
        fixed
    );

    printf(
        "sizes=%u,%u,%u,%u\n",
        sizeof(char),
        sizeof(short),
        sizeof(int),
        sizeof(struct Point)
    );

    printf(
        "integers=%u,%d,%u,%d,%u\n",
        small,
        negative,
        medium,
        large,
        flags
    );

    return result;
}