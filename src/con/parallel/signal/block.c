#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>

void on_sigint_handler(int s)
{
    write(1, "!", 1);
}

int main()
{
    sigset_t set;

    signal(SIGINT, on_sigint_handler);

    // clear set and add SIGINT into set
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    for (int k = 0; k < 1000; k++) {
        // block SIGINT signal
        sigprocmask(SIG_BLOCK, &set, NULL);

        for (int i = 0; i < 5; i++) {
            write(1, "*", 1);
            sleep(1);
        }

        write(1, "\n", 1);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
    }

    exit(0);
}