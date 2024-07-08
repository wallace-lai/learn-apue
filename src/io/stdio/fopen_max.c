#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    FILE *fp = NULL;
    int count = 0;

    while (1) {
        fp = fopen("fopen.c", "r");
        if (fp == NULL) {
            perror("fopen()");
            break;
        }
        count++;
    }
    printf("count = %d\n", count);

    return 0;
}
