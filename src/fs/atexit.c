#include <stdio.h>
#include <stdlib.h>

static void f1(void)
{
    puts("f1 working !");
}

static void f2(void)
{
    puts("f2 working !");
}

static void f3(void)
{
    puts("f3 working !");
}

int main()
{
    puts("BEGIN ...");
    atexit(f1);
    atexit(f2);
    atexit(f3);
    puts("END ...");

    return 0;
}