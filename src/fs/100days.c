#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESIZE 128
int main()
{
    time_t stamp;
    struct tm *tm;
    char timestr[TIMESIZE];

    stamp = time(NULL);
    tm = localtime(&stamp);
    strftime(timestr, TIMESIZE, "Now : %Y-%m-%d", tm);
    puts(timestr);

    tm->tm_mday += 100;
    mktime(tm);
    strftime(timestr, TIMESIZE, "100 Days After : %Y-%m-%d", tm);
    puts(timestr);

    exit(0);
}