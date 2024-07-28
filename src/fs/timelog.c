#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define FNAME "/tmp/out"
#define BUFSIZE 128

int main()
{
    static char buffer[BUFSIZE];

    FILE *fp = fopen(FNAME, "a+");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }

    unsigned count = 0;
    time_t stamp = 0;
    static struct tm result;
    while (1) {
        stamp = time(NULL);
        localtime_r(&stamp, &result);
        memset(buffer, 0, BUFSIZE);
        (void)strftime(buffer, BUFSIZE, "%Y-%m-%d %H:%M:%S", &result);
        fprintf(stdout, "%u %s\n", count, buffer);
        fprintf(fp, "%u %s\n", count, buffer);
        fflush(NULL);

        count++;
        sleep(1);
    }

    exit(0);
}