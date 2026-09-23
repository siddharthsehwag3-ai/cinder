int main()
{
    char text[64];
    int length = 0;
    int character;

    character = getchar();

    while (
        character != -1 &&
        character != '\n' &&
        length < 63
    ) {
        text[length] =
            (char)character;

        length++;
        character = getchar();
    }

    text[length] = '\0';

    printf(
        "input=%s\n",
        text
    );

    printf(
        "length=%d\n",
        length
    );

    return length;
}