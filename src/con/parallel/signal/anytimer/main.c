#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void f1(void *p)
{
    printf("f1() : %s\n", p);
}

static void f2(void *p)
{
    printf("f2() : %s\n", p);
}

int main()
{
    puts("Begin ...");

    // 5, f1, "aaa"
    // 2, f2, "bbb"
    // 7, f1, "ccc"

    puts("End ...");

    while (1) {
        pause();
    }

    exit(0);
}