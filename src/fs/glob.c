#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAT "/etc/*"

static int errfunc_(const char *errpath, int eerrno)
{
    puts(errpath);
    fprintf(stderr, "ERROR MSG : %s\n", strerror(eerrno));
}

int main(int argc, char **argv)
{
    int err = 0;
    glob_t res = { 0 };

    err = glob(PAT, 0, NULL, &res);
    if (err) {
        printf("ERROR CODE : %d\n", err);
        exit(1);
    }

    for (int i = 0; i < res.gl_pathc; i++) {
        puts(res.gl_pathv[i]);
    }

    globfree(&res);
    exit(0);
}