#include <stdio.h>


// Файл содержит только символы-цифры, увеличить их значения на 1 за один просмотр файла (цифру ‘9’ заменить на ‘0’)
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("argc != 2\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "r+");

    if (!f)
    {
        printf("Cant open file\n");
        return -2;
    }

    int c;

    while ((c = fgetc(f)) != EOF)
    { 
        fseek(f, -sizeof(char), SEEK_CUR);

        if (c == '9')
            c='0';
        else 
            c++;

        fputc(c, f);
    }

    fclose(f);

    return 0;
}
