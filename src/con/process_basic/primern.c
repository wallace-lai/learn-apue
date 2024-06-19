#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

#define BEG 30000000
#define END 30000200
#define N   3

static int is_primer(int num)
{
    for (int i = 2; i < num / 2; i++) {
        if (num % i == 0) {
            return 0;
        }
    }

    return 1;
}

int main()
{
    for (int n = 0; n < N; n++) {
        pid_t pid = fork();
        if (pid < 0) {
            int count = n;
            while (count--) {
                wait(NULL);
            }

            perror("fork()");
            exit(1);
        }

        if (pid == 0) {
            // child
            for (int i = BEG + n; i <= END; i += N) {
                if (is_primer(i)) {
                    printf("[%d] %d is primer number\n", n, i);
                }
            }

            exit(0);
        }
    }

    for (int n = 0; n < N; n++) {
        wait(NULL);
    }

    return 0;
}