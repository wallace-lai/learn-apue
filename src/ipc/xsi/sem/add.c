#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define PROCNUM     20
#define FNAME       "/tmp/out"
#define LINESIZE    1024

static int semid;

static void p()
{
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = 0;

    while (semop(semid, &buf, 1) < 0) {
        if (errno != EINTR || errno != EAGAIN) {
            perror("semop()");
            exit(1);
        }
    }

    return;
}

static void v()
{
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = 0;

    if (semop(semid, &buf, 1) < 0) {
        perror("semop()");
        exit(1);
    }

    return;
}

static void func_add(void)
{
    FILE *fp = fopen(FNAME, "r+");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }

    static char linebuf[LINESIZE];

    p();

    fgets(linebuf, LINESIZE, fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", atoi(linebuf) + 1);
    fflush(fp);

    v();

    fclose(fp);
    return;
}

int main()
{
    semid = semget(IPC_PRIVATE, 1, 0600);
    if (semid < 0) {
        perror("semget()");
        exit(1);
    }

    int ret = semctl(semid, 0, SETVAL, 1);
    if (ret < 0) {
        perror("semctl()");
        exit(1);
    }

    for (int i = 0; i < PROCNUM; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(1);
        }

        if (pid == 0) {
            // child
            func_add();
            exit(0);
        }
    }

    for (int i = 0; i < PROCNUM; i++) {
        wait(NULL);
    }

    semctl(semid, 0, IPC_RMID);
}