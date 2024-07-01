#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mytbf.h"

#define CPS     64
#define BURST   1024
#define BUFSIZE 1024

int main(int argc, char **argv)
{
    int ret;
    int sfd;
    int dfd = 1;
    ssize_t len;
    ssize_t pos;
    static char buffer[BUFSIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    mytbf_t *tbf = mytbf_init(CPS, BURST);
    if (tbf == NULL) {
        fprintf(stderr, "mytbf_init() failed.");
        exit(1);
    }

    do {
        sfd = open(argv[1], O_RDONLY);
        if (sfd < 0) {
            if (errno != EINTR) {
                perror("open()");
                exit(1);
            }
        }
    } while (sfd < 0);

    while (1) {
        // fetch token from tbf
        unsigned size = mytbf_fetch_token(tbf, BUFSIZ);

        while ((len = read(sfd, buffer, size)) < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("read()");
            break;
        }
        if (len == 0) {
            break;
        }

        // return token if lefted
        if (size - len > 0) {
            mytbf_return_token(tbf, size - len);
        }

        pos = 0;
        while (len > 0) {
            ret = write(dfd, buffer + pos, len);
            if (ret < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("write()");
                exit(1);
            }

            pos += ret;
            len -= ret;
        }
    }

    mytbf_deinit(tbf);

    close(sfd);
    exit(0);
}