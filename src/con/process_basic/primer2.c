#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

#define BEG 30000000
#define END 30000200

int main()
{
    for (int i = BEG; i <= END; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(1);
        }

        if (pid == 0) {
            int mark = 1;
            for (int p = 2; p < i / 2; p++) {
                if (i % p == 0) {
                    mark = 0;
                    break;
                }
            }

            if (mark) {
                printf("%d is a primer.\n", i);
            }

            exit(0);
        }
    }

    for (int i = BEG; i <= END; i++) {
        wait(NULL);
    }

    return 0;
}