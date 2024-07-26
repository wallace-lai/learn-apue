#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <glob.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>

#define PATHSIZE 1024

// pending, BUGs here
// P152
static int path_noloop(const char *path)
{
    char *pos = NULL;

    pos = strchr(path, '/');
    if (pos == NULL) {
        exit(1);
    }


    if (strcmp(pos + 1, ".") == 0 || strcmp(pos + 1, "..") == 0) {
        return 0;
    }


    return 1;
}

static uint64_t mydu(const char *path)
{
    int ret;
    static glob_t globres;
    static struct stat statres;
    char nextpath[PATHSIZE];

    ret = lstat(path, &statres);
    if (ret < 0) {
        perror("lstat()");
        return 0;
    }

    if (!S_ISDIR(statres.st_mode)) {
        return (statres.st_blocks);
    }

    strncpy(nextpath, path, PATHSIZE);
    strncat(nextpath, "/*", strlen(nextpath) + strlen("/*") + 1);
    ret = glob(nextpath, 0, NULL, &globres);
    if (ret != 0) {
        perror("glob()");
        return 0;
    }
    // printf("path = %s\n", path);
    // printf("* subs :\n");
    // for (int i = 0; i < globres.gl_pathc; i++) {
    //     printf("%s\n", globres.gl_pathv[i]);
    // }

    strncpy(nextpath, path, PATHSIZE);
    strncat(nextpath, "/.*", strlen(nextpath) + strlen("/.*") + 1);
    ret = glob(nextpath, GLOB_APPEND, NULL, &globres);
    // printf("\n.* subs :\n");
    // for (int i = 0; i < globres.gl_pathc; i++) {
    //     printf("%s\n", globres.gl_pathv[i]);
    // }

    uint64_t sum = statres.st_blocks;
    for (int i = 0; i < globres.gl_pathc; i++) {
        if (path_noloop(globres.gl_pathv[i])) {
            sum += mydu(globres.gl_pathv[i]);
        }
    }

    return sum;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    printf("%ld\n", mydu(argv[1]) / 2);
    exit(0);
}