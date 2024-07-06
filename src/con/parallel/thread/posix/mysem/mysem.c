#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "mysem.h"


struct mysem_s {
    int value;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
};

mysem_t *mysem_init(int value)
{
    struct mysem_s *sem = NULL;

    sem = (struct mysem_s *)malloc(sizeof(struct mysem_s));
    if (sem == NULL) {
        return sem;
    }

    sem->value = value;
    pthread_mutex_init(&sem->mtx, NULL);
    pthread_cond_init(&sem->cond, NULL);
    return sem;
}

int mysem_add(mysem_t *sem, int num)
{
    if (sem == NULL) {
        return -1;
    }

    struct mysem_s *s = (struct mysem_s *)sem;
    pthread_mutex_lock(&s->mtx);
    s->value = s->value + num;
    pthread_cond_broadcast(&s->cond);
    pthread_mutex_unlock(&s->mtx);
    return num;
}

int mysem_sub(mysem_t *sem, int num)
{
    if (sem == NULL) {
        return -1;
    }

    struct mysem_s *s = (struct mysem_s *)sem;
    pthread_mutex_lock(&s->mtx);
    while (s->value < num) {
        pthread_cond_wait(&s->cond, &s->mtx);
    }

    s->value = s->value - num;
    pthread_mutex_unlock(&s->mtx);
    return num;
}

int mysem_destroy(mysem_t *sem)
{
    if (sem == NULL) {
        return -1;
    }

    struct mysem_s *s = (struct mysem_s *)sem;
    pthread_mutex_destroy(&s->mtx);
    pthread_cond_destroy(&s->cond);
    free(s);
    return 0;
}