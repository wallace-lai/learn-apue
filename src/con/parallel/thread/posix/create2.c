#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

static void *routine(void *ctx)
{
    int i = 5;
    printf("Addr : %p\n", &i);
}

int main()
{
    int i;
    int err;
    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024 * 1024);

    for (i = 0; true; i++) {
        err = pthread_create(&tid, &attr, routine, NULL);
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            break;
        }
    }

    fprintf(stderr, "i = %d\n", i);
    pthread_attr_destroy(&attr);
    exit(0);
}