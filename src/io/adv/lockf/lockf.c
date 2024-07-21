#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#define PROC_NUM   20
#define FNAME      "/tmp/out"
#define LINE_SIZE  512

static void *func_add(void)
{
    FILE *f = fopen(FNAME, "r+");
    if (f == NULL) {
        perror("fopen()");
        exit(1);
    }

    int fd = fileno(f);
    char linebuf[LINE_SIZE];

    lockf(fd, F_LOCK, 0);    // 加锁

    fgets(linebuf, LINE_SIZE, f);
    int num = atoi(linebuf) + 1;
    fseek(f, 0, SEEK_SET);
    fprintf(f, "%d\n", num);
    fflush(f);

    lockf(fd, F_ULOCK, 0);   // 解锁

    fclose(f);  // 为了防止意外解锁，必须在解锁后close
}

int main()
{
    pid_t pid;

    for (int i = 0; i < PROC_NUM; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(1);
        }

        if (pid == 0) {
            func_add();
            exit(0);
        }
    }

    for (int i = 0; i < PROC_NUM; i++) {
        wait(NULL);
    }

    exit(0);
}