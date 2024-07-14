#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "relayer.h"

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"
#define TTY3 "/dev/tty10"
#define TTY4 "/dev/tty9"

static int open_relayer_tty(const char *tty1, const char *tty2, int *pfd1, int *pfd2)
{
    int fd1;
    int fd2;

    fd1 = open(tty1, O_RDWR);
    if (fd1 < 0) {
        perror("open()");
        return -1;
    }
    fprintf(fd1, "%s\n", tty1);

    fd2 = open(tty2, O_RDWR);
    if (fd2 < 0) {
        close(fd1);
        perror("open()");
        return -1;
    }
    fprintf(fd2, "%s\n", tty2);

    *pfd1 = fd1;
    *pfd2 = fd2;
    return 0;
}

int main(int argc, char **argv)
{
    int ret;

    int fd1;
    int fd2;
    ret = open_relayer_tty(TTY1, TTY2, &fd1, &fd2);
    if (ret < 0) {
        exit(1);
    }

    int job1 = relayer_add_job(fd1, fd2);
    if (job1 < 0) {
        fprintf(stderr, "relayer_add_job() : %s\n", strerror(-job1));
        exit(1);
    }

    int fd3;
    int fd4;
    ret = open_relayer_tty(TTY3, TTY4, &fd3, &fd4);
    if (ret < 0) {
        exit(1);
    }

    int job2 = relayer_add_job(fd3, fd4);
    if (job2 < 0) {
        fprintf(stderr, "relayer_add_job() : %s\n", -job2);
        exit(1);
    }

    while (1) {
        pause();
    }

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
    exit(0);
}