// rcver.c

#include <stdio.h>
#include <stdlib.h>

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

    int msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid < 0) {
        perror("msgget()");
        exit(1);
    }

    msg rbuf;
    while (1) {
        ssize_t len = msgrcv(msgid, &rbuf, sizeof(msg) - sizeof(long), 0, 0);
        if (len < 0) {
            perror("msgrcv()");
            exit(1);
        }

        printf("NAME = %s\nMATH = %d\nChinese = %d\n", rbuf.name, rbuf.math, rbuf.chinese);
    }

    msgctl(msgid, IPC_RMID, NULL);
    exit(0);
}