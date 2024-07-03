#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BEG 30000000
#define END 30000200
#define THREAD_NUM (END - BEG + 1)

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

    return NULL;
}

int main()
{
    int err;
    pthread_t tid[THREAD_NUM];

    for (int i = BEG; i <= END; i++) {
        err = pthread_create(&tid[i - BEG], NULL, routine, (void *)i);
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            exit(1);
        }
    }

    for (int i = BEG; i <= END; i++) {
        pthread_join(tid[i - BEG], NULL);
    }

    exit(0);
}