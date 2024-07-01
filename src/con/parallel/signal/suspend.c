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
    sigset_t old_set;
    sigset_t save_set;

    signal(SIGINT, on_sigint_handler);

    // clear set and add SIGINT into set
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_UNBLOCK, &set, &save_set);
    for (int k = 0; k < 1000; k++) {
        // block SIGINT signal
        sigprocmask(SIG_BLOCK, &set, &old_set);

        for (int i = 0; i < 5; i++) {
            write(1, "*", 1);
            sleep(1);
        }

        write(1, "\n", 1);
        sigsuspend(&old_set);
        // sigprocmask(SIG_SETMASK, &old_set, NULL);
        // pause();
    }

    sigprocmask(SIG_SETMASK, &save_set, NULL);
    exit(0);
}