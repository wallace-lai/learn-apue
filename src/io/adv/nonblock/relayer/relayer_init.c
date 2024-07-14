#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "relayer_init.h"
#include "relayer.h"

static pthread_t tid;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

static void relayer_init_impl(void)
{
    int err;

    err = pthread_create(&tid, NULL, relayer_routine, NULL);
    if (err) {
        fprintf(stderr, "pthread_create() : %s\n", strerror(err));
        exit(1);
    }
}

int relayer_init(void)
{
    pthread_once(&init_once, relayer_init_impl);
}

int relayer_exit(void)
{
    pthread_cancel(tid);
    pthread_join(tid, NULL);
}