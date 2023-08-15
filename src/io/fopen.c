#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    FILE *fp = fopen("noexist", "w+");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }
    puts("OK");

    fclose(fp);
    return 0;
}

// int main()
// {
//     int *ptr = malloc(sizeof(int));
//     return 0;
// }