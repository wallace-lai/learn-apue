#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

#include <sys/types.h>

#define PAT "/etc"

int main()
{
    DIR *dp = NULL;
    struct dirent *curr = NULL;

    dp = opendir(PAT);
    if (dp == NULL) {
        perror("opendir()");
        exit(1);
    }

    while ((curr = readdir(dp)) != NULL) {
        puts(curr->d_name);
    }


    closedir(dp);
    exit(0);
}