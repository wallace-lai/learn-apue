#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    puts("my date begin ...");

    fflush(NULL);
    execl("/usr/bin/date", "date", "+%s", NULL);
    perror("execl()");
    exit(1);

    puts("my date end ...");
    exit(0);
}