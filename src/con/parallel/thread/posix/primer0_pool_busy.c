#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define BEG 30000000
#define END 30000200

#define THREAD_NUM 4

static int num;
static int idx[THREAD_NUM];
static pthread_mutex_t mtx;

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
        pthread_mutex_lock(&mtx);

        // 当前无任务
        if (num == 0) {
            pthread_mutex_unlock(&mtx);
            sched_yield();
            continue;
        }

        // 所有任务执行结束
        if (num == -1) {
            pthread_mutex_unlock(&mtx);
            break;
        }

        // 当前有任务，执行之
        copy = num;
        num = 0;   // num清零表示任务消费完毕
        pthread_mutex_unlock(&mtx);

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

    // 初始化num和mtx
    num = 0;
    pthread_mutex_init(&mtx, NULL);


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
        while (1) {
            pthread_mutex_lock(&mtx);

            // 上一次下发的任务还未执行完毕
            if (num != 0) {
                pthread_mutex_unlock(&mtx);
                sched_yield();  // 让出调度器，让线程有机会拿到锁并执行任务
                continue;
            }

            // 当前任务成功下发
            num = i;
            pthread_mutex_unlock(&mtx);
            break;
        }
    }

    // 下发完毕后查看num是否为0，若是则下发-1
    while (1) {
        pthread_mutex_lock(&mtx);

        // 还有任务未执行完毕
        if (num != 0) {
            pthread_mutex_unlock(&mtx);
            sched_yield();
            continue;
        }

        // 成功下发-1
        num = -1;
        pthread_mutex_unlock(&mtx);
        break;
    }

    // 等待线程执行完毕
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }
    exit(0);
}
