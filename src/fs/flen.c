#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

static off_t flen(const char *pathname)
{
    struct stat res;
    if (stat(pathname, &res) < 0) {
        perror("stat()");
        exit(1);
    }

    return res.st_size;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    printf("Total size : %ld\n", flen(argv[1]));
    exit(0);
}
