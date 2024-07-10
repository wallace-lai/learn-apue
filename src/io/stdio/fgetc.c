#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("fopen()");
        exit(1);
    }

    int ch;
    unsigned count = 0;
    while ((ch = fgetc(f)) != EOF) {
        count++;
    }
    printf("File size : %u\n", count);

    fclose(f);
    exit(0);
}