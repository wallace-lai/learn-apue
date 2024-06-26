#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CPS     32
#define BUFSIZE CPS
#define BURST   10

static volatile unsigned token = 0;

static void on_alarm_handler(int s)
{
    // set next alarm signal
    alarm(1);

    // you got one token
    token++;
    if (token > BURST) {
        token = BURST;
    }
}

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

    signal(SIGALRM, on_alarm_handler);
    alarm(1);

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

        // flow control
        while (token <= 0) {
            pause();
        }
        token--;

        while ((len = read(sfd, buffer, BUFSIZE)) < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("read()");
            break;
        }
        if (len == 0) {
            break;
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

    close(sfd);
    exit(0);
}