#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

static int ftype(const char *fname)
{
    struct stat res;

    if (stat(fname, &res) < 0) {
        perror("stat");
        exit(1);
    }

    int ret = 0;
    if (S_ISREG(res.st_mode)) {
        ret = '-';
    } else if (S_ISDIR(res.st_mode)) {
        ret = 'd';
    } else if (S_ISCHR(res.st_mode)) {
        ret = 'c';
    } else if (S_ISBLK(res.st_mode)) {
        ret = 'b';
    } else if (S_ISFIFO(res.st_mode)) {
        ret = 'p';
    } else if (S_ISSOCK(res.st_mode)) {
        ret = 's';
    } else if (S_ISLNK(res.st_mode)) {
        ret = 'l';
    } else {
        ret = '?';
    }

    return ret;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    printf("File Type : %c\n", ftype(argv[1]));
    exit(0);
}