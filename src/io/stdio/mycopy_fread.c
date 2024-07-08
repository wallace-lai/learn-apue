#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    char buffer[BUFSIZE];
    FILE *fpSrc = NULL;
    FILE *fpDst = NULL;

    if (argc < 3) {
        fprintf(stderr, "Usage : %s <src_file> <dest_file>\n", argv[0]);
        exit(1);
    }

    fpSrc = fopen(argv[1], "r");
    if (fpSrc == NULL) {
        perror("fopen()");
        exit(1);
    }

    fpDst = fopen(argv[2], "w");
    if (fpDst == NULL) {
        fclose(fpSrc);
        perror("fopen()");
        exit(1);
    }

    // while (1) {
    //     ch = fgetc(fpSrc);
    //     if (ch == EOF) {
    //         break;
    //     }
    //     fputc(ch, fpDst);
    // }

    // while (fgets(buffer, BUFSIZ, fpSrc) != NULL) {
    //     fputs(buffer, fpDst);
    // }

    int n = 0;
    while ((n = fread(buffer, 1, BUFSIZ, fpSrc)) > 0) {
        fwrite(buffer, 1, n, fpDst);
    }

    fclose(fpDst);
    fclose(fpSrc);

    return 0;
}
