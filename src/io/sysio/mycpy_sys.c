#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_BUFSIZE 1024

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    int sfd = open(argv[1], O_RDONLY);
    if (sfd < 0) {
        perror("open()");
        exit(1);
    }

    int dfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (dfd < 0) {
        close(sfd);
        perror("open()");
        exit(1);
    }

    int ret;
    ssize_t len = 0;
    char buffer[MAX_BUFSIZE];
    while (1) {
        len = read(sfd, buffer, MAX_BUFSIZE);
        if (len < 0) {
            perror("read()");
            break;
        }

        if (len == 0) {
            break;
        }

        int pos = 0;
        while (len > 0) {
            ret = write(dfd, buffer + pos, len);
            if (ret < 0) {
                perror("write()");
                close(dfd);
                close(sfd);
                exit(1);
            }

            pos += ret;
            len -= ret;
        }
    }

    close(dfd);
    close(sfd);
    exit(0);
}