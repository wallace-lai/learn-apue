#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>

static volatile int need_loop = 1;

static void on_alarm_handler(int s)
{
    need_loop = 0;
}

int main()
{
    alarm(5);
    signal(SIGALRM, on_alarm_handler);

    unsigned long long count = 0;
    while (need_loop) {
        count++;
    }

    printf("5sec by alarm : %lld\n", count);
    exit(0);
}