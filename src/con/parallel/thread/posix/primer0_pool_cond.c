#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define BEG 30000000
#define END 30000200

#define THREAD_NUM 4

static int num;
static pthread_mutex_t mtx_num = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_num = PTHREAD_COND_INITIALIZER;

static int idx[THREAD_NUM];

static int is_primer(int i)
{
    for (int p = 2; p < i / 2; p++) {
        if (i % p == 0) {
            return 0;
        }
    }

    return 1;
}

static void *routine(void *ctx)
{
    int copy;
    int idx = *(int *)ctx;

    while (1) {
        pthread_mutex_lock(&mtx_num);
        while (num == 0) {
            pthread_cond_wait(&cond_num, &mtx_num);
        }

        if (num == -1) {
            pthread_mutex_unlock(&mtx_num);
            break;
        }

        copy = num;
        num = 0;
        pthread_cond_broadcast(&cond_num);
        pthread_mutex_unlock(&mtx_num);

        if (is_primer(copy)) {
            fprintf(stdout, "[%d] %d is primer\n", idx, copy);
        }
    }

    pthread_exit(NULL);
}

int main()
{
    int err;
    pthread_t tid[THREAD_NUM];

    // 初始化num
    num = 0;

    // 创建四个线程
    for (int i = 0; i < THREAD_NUM; i++) {
        idx[i] = i;

        err = pthread_create(&tid[i], NULL, routine, (void *)(&idx[i]));
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            exit(1);
        }
    }

    // 下发任务
    for (int i = BEG; i <= END; i++) {

        pthread_mutex_lock(&mtx_num);
        while (num != 0) {
            pthread_cond_wait(&cond_num, &mtx_num);
        }

        num = i;
        pthread_cond_signal(&cond_num);
        pthread_mutex_unlock(&mtx_num);
    }

    // 下发完毕后查看num是否为0，若是则下发-1
    pthread_mutex_lock(&mtx_num);
    while (num != 0) {
        pthread_cond_wait(&cond_num, &mtx_num);
    }

    num = -1;
    pthread_cond_broadcast(&cond_num);
    pthread_mutex_unlock(&mtx_num);


    // 等待线程执行完毕
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mtx_num);
    pthread_cond_destroy(&cond_num);

    exit(0);
}
