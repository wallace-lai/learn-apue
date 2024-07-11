#include <stdio.h>
#include <string.h>
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

    int count = -1;
    // !!! we must initialize line_buffer and line_length here
    char *line_buffer = NULL;
    size_t line_length = 0;
    while (1) {
        count++;
        if (getline(&line_buffer, &line_length, f) < 0) {
            break;
        }
        printf("Line %d, length  %ld, buffer length %ld\n", count, strlen(line_buffer), line_length);
        free(line_buffer);
    }

    fclose(f);
    exit(0);
}