#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <shadow.h>
#include <crypt.h>

int main(int argc, char *argv[])
{
    char *input = NULL;
    struct spwd *shadowline = NULL;
    char *crypted_pass = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    input = getpass("Password :");
    shadowline = getspnam(argv[1]);
    crypted_pass = crypt(input, shadowline->sp_pwdp);
    if (strcmp(shadowline->sp_pwdp, crypted_pass) == 0) {
        puts("OK!");
    } else {
        puts("FAILED!");
    }

    exit(0);
}