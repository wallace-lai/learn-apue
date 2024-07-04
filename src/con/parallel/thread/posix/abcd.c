#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUM 4

static int idx[THREAD_NUM];
static pthread_mutex_t mtx[THREAD_NUM];

static int next(int i)
{
    if (i + 1 == THREAD_NUM) {
        return 0;
    }

    return i + 1;
}

static void *routine(void *ctx)
{
    int i = *(int *)ctx;
    char c = 'A' + i;

    while (1) {
        pthread_mutex_lock(&mtx[i]);
        write(STDOUT_FILENO, &c, 1);
        pthread_mutex_unlock(&mtx[next(i)]);
    }

    pthread_exit(NULL);
}

int main()
{
    int err;
    pthread_t tid[THREAD_NUM];

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_mutex_init(&mtx[i], NULL);
        pthread_mutex_lock(&mtx[i]);

        idx[i] = i;
        err = pthread_create(&tid[i], NULL, routine, (void *)(&idx[i]));
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            exit(1);
        }
    }

    pthread_mutex_unlock(&mtx[0]);

    alarm(2);
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }
    exit(0);
}