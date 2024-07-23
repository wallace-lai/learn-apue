#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    int ret;

    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        perror("open()");
        exit(1);
    }

    off_t len = 5LL * 1024 * 1024 * 1024 - 1;
    ret = lseek(fd, len, SEEK_SET);
    if (ret < 0) {
        perror("lseek()");
        exit(1);
    }

    write(fd, "\0", 1);
    close(fd);

    exit(0);
}