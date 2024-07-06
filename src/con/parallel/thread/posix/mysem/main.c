#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "mysem.h"

#define BEG 30000000
#define END 30000200
#define THREAD_NUM 4

static mysem_t *sem;

static void *routine(void *ctx)
{
    int i = (int)ctx;
    int mark = 1;

    for (int p = 2; p < i / 2; p++) {
        if (i % p == 0) {
            mark = 0;
            break;
        }
    }

    if (mark) {
        printf("%d is a primer.\n", i);
    }

    mysem_add(sem, 1);
    pthread_exit(NULL);
}

int main()
{
    int err;
    void *ptr;
    pthread_t tid[THREAD_NUM];

    sem = mysem_init(THREAD_NUM);
    if (sem == NULL) {
        fprintf(stderr, "mysem_init() failed\n");
        exit(1);
    }

    for (int i = BEG; i <= END; i++) {
        mysem_sub(sem, 1);

        // num[i - BEG] = i;
        err = pthread_create(&tid[i - BEG], NULL, routine, (void *)(i));
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            exit(1);
        }
    }

    for (int i = BEG; i <= END; i++) {
        pthread_join(tid[i - BEG], NULL);
    }

    mysem_destroy(sem);
    exit(0);
}