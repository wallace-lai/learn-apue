#include <stdio.h>
#include <stdlib.h>

int main()
{
    puts(getenv("HOME"));
    setenv("PNAME", "getenv.c", 1);
    puts(getenv("PNAME"));

    getchar();
    exit(0);
}