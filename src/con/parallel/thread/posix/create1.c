#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static void *routine(void *ctx)
{
    fprintf(stdout, "[INFO] thread %d is working ...\n", (int)pthread_self());
    pthread_exit(0);
}

int main()
{
    puts("Begin ...");

    pthread_t tid;
    int err = pthread_create(&tid, NULL, routine, NULL);
    if (err) {
        fprintf(stderr, "pthread_create() : %s\n", strerror(err));
        exit(1);
    }

    pthread_join(tid, NULL);
    puts("End ...");
    exit(0);
}