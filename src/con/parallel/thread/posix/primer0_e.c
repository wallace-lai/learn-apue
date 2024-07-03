#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BEG 30000000
#define END 30000200
#define THREAD_NUM (END - BEG + 1)


typedef struct thread_argument_s {
    int n;
} thread_argument_t;

static void *routine(void *ctx)
{
    thread_argument_t *p = (thread_argument_t *)ctx;
    int i = p->n;
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

    pthread_exit(p);
}

int main()
{
    int err;
    void *ptr;
    pthread_t tid[THREAD_NUM];

    for (int i = BEG; i <= END; i++) {
        thread_argument_t *p = (thread_argument_t *)malloc(sizeof(thread_argument_t));
        if (p == NULL) {
            perror("malloc()");
            exit(1);
        }
        p->n = i;

        err = pthread_create(&tid[i - BEG], NULL, routine, p);
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            exit(1);
        }
    }

    for (int i = BEG; i <= END; i++) {
        pthread_join(tid[i - BEG], &ptr);
        free(ptr);
    }

    exit(0);
}