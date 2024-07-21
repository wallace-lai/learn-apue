#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open()");
        exit(1);
    }

    struct stat statres = {0};
    if (fstat(fd, &statres) < 0) {
        perror("fstat()");
        exit(1);
    }


    char *str = mmap(NULL, statres.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (str == MAP_FAILED) {
        perror("mmap()");
        exit(1);
    }
    close(fd);

    unsigned count = 0;
    for (int i = 0; i < statres.st_size; i++) {
        if (str[i] == 'a') {
            count++;
        }
    }
    printf("count = %d\n", count);

    munmap(str, statres.st_size);
    exit(0);

}