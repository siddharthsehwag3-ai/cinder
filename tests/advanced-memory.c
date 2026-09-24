int global_numbers[5] = {
    3,
    5,
    8,
    13,
    21
};

char global_message[] =
    "Cinder memory works";

int sum_values(
    int values[],
    int count
)
{
    int index = 0;
    int total = 0;

    while (index < count) {
        total += values[index];
        index++;
    }

    return total;
}

int matrix_total(
    int matrix[2][3]
)
{
    int row = 0;
    int column = 0;
    int total = 0;

    while (row < 2) {
        column = 0;

        while (column < 3) {
            total +=
                matrix[row][column];

            column++;
        }

        row++;
    }

    return total;
}

int pointer_total(
    int *first,
    int count
)
{
    int total = 0;
    int index = 0;

    while (index < count) {
        total += *(first + index);
        index++;
    }

    return total;
}

int main()
{
    int local_numbers[5] = {
        2,
        4,
        6,
        8,
        10
    };

    int matrix[2][3] = {
        {
            1,
            2,
            3
        },
        {
            4,
            5,
            6
        }
    };

    char local_message[] =
        "arrays + pointers + strings";

    int *pointer =
        &local_numbers[0];

    int global_sum =
        sum_values(
            global_numbers,
            5
        );

    int local_sum =
        pointer_total(
            pointer,
            5
        );

    int grid_sum =
        matrix_total(
            matrix
        );

    printf(
        "%s\n",
        global_message
    );

    printf(
        "%s\n",
        local_message
    );

    printf(
        "global=%d local=%d matrix=%d\n",
        global_sum,
        local_sum,
        grid_sum
    );

    printf(
        "pointer[2]=%d sizeof-array=%u\n",
        *(pointer + 2),
        sizeof(local_numbers)
    );

    return
        global_sum +
        local_sum +
        grid_sum;
}