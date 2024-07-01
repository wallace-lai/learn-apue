#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>

#define MYRTSIG (SIGRTMIN + 6)

void on_myrtsig_handler(int s)
{
    write(1, "!", 1);
}

int main()
{
    sigset_t set;
    sigset_t old_set;
    sigset_t save_set;

    signal(MYRTSIG, on_myrtsig_handler);

    sigemptyset(&set);
    sigaddset(&set, MYRTSIG);
    sigprocmask(SIG_UNBLOCK, &set, &save_set);

    sigprocmask(SIG_BLOCK, &set, &old_set);
    for (int k = 0; k < 1000; k++) {
        for (int i = 0; i < 5; i++) {
            write(1, "*", 1);
            sleep(1);
        }
        write(1, "\n", 1);
        sigsuspend(&old_set);
    }

    sigprocmask(SIG_SETMASK, &save_set, NULL);
    exit(0);
}