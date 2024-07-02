#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static void clean(void *ctx)
{
    puts(ctx);
}

static void *routine(void *ctx)
{
    printf("[INFO] thread %d is working ...\n", (int)pthread_self());

    // pthread_cleanup_push是宏
    pthread_cleanup_push(clean, "clean up 1");
    pthread_cleanup_push(clean, "clean up 2");
    pthread_cleanup_push(clean, "clean up 3");

    printf("[INFO] thread %d cleanup push over.\n", (int)pthread_self());

    // pthread_cleanup_pop入参为1表示需要取出钩子函数并执行
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);

    pthread_exit(NULL);
}

int main()
{
    puts("[INFO] begin ...");

    pthread_t tid;
    int err = pthread_create(&tid, NULL, routine, NULL);
    if (err) {
        fprintf(stderr, "pthread_create() : %s\n", strerror(err));
        exit(1);
    }

    pthread_join(tid, NULL);
    puts("[INFO] end ...");
    exit(0);
}