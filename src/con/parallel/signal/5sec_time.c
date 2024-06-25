#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main()
{
    unsigned long long count = 0;
    time_t end = time(NULL) + 5;

    while (time(NULL) < end) {
        count++;
    }

    printf("5sec by time : %lld\n", count);
    exit(0);
}
