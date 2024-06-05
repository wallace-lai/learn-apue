#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFSIZE 1024

int main()
{
    int pd[2];
    pid_t pid;
    char buffer[BUFSIZE];

    if (pipe(pd) < 0) {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }

    if (pid == 0) {
        // child
        close(pd[1]);
        int len = read(pd[0], buffer, BUFSIZE);
        write(1, buffer, len);
        close(pd[0]);
        exit(0);
    } else {
        // parent
        const char *msg = "hello message from parent.\n";
        close(pd[0]);
        write(pd[1], msg, strlen(msg));
        close(pd[1]);
        wait(NULL);
        exit(0);
    }

    return 0;
}