// snder.c

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"

int main()
{
    key_t key = ftok(KEYPATH, KEYPROJ);
    if (key < 0) {
        perror("ftok()");
        exit(1);
    }

    int msgid = msgget(key, 0);
    if (msgid < 0) {
        perror("msgget()");
        exit(1);
    }

    srand(time(NULL));

    msg sbuf;
    sbuf.mtype = 1;
    strncpy(sbuf.name, "Alice", NAME_SIZE);
    sbuf.math = rand() % 100;
    sbuf.chinese = rand() % 100;

    int ret = msgsnd(msgid, &sbuf, sizeof(sbuf) - sizeof(long), 0);
    if (ret < 0) {
        perror("msgsnd()");
        exit(1);
    }

    puts("OK");
    exit(0);
}