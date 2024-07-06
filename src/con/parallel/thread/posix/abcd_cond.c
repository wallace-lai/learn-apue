#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#define THREAD_NUM 4

static int num;
static pthread_mutex_t mtx_num = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_num = PTHREAD_COND_INITIALIZER;

static int idx[THREAD_NUM];
static pthread_t tid[THREAD_NUM];

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
        pthread_mutex_lock(&mtx_num);
        while (num != i) {
            pthread_cond_wait(&cond_num, &mtx_num);
        }

        write(STDOUT_FILENO, &c, 1);
        num = next(num);
        pthread_cond_broadcast(&cond_num);
        pthread_mutex_unlock(&mtx_num);
    }

    pthread_exit(NULL);
}

static void on_quit_handler(int s)
{
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mtx_num);
    pthread_cond_destroy(&cond_num);
    exit(0);
}

int main()
{
    int err;
    // struct sigaction oact;
    // struct sigaction nact;

    // nact.sa_handler = on_quit_handler;
    // sigemptyset(&nact.sa_mask);
    // sigaddset(&nact.sa_mask, SIGQUIT);
    // sigaddset(&nact.sa_mask, SIGTERM);
    // sigaddset(&nact.sa_mask, SIGILL);
    // nact.sa_flags = 0;

    // err = sigaction(SIGINT, &nact, &oact);
    // if (err < 0) {
    //     perror("sigaction()");
    //     exit(0);
    // }

    num = 0;
    for (int i = 0; i < THREAD_NUM; i++) {
        idx[i] = i;
        err = pthread_create(&tid[i], NULL, routine, (void *)(&idx[i]));
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            exit(1);
        }
    }

    alarm(2);

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mtx_num);
    pthread_cond_destroy(&cond_num);
    exit(0);
}