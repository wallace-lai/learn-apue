#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "mypipe.h"

struct mypipe {
    pthread_mutex_t mtx;
    pthread_cond_t cond;

    int head;
    int tail;
    int size;
    int reader_count;
    int writer_count;
    char data[PIPESIZE];
};

mypipe_t *mypipe_init(void)
{
    struct mypipe *pipe;

    pipe = malloc(sizeof(*pipe));
    if (pipe == NULL) {
        return NULL;
    }

    memset(pipe, 0, sizeof(*pipe));
    pthread_mutex_init(&pipe->mtx);
    pthread_cond_init(&pipe->cond);

    return pipe;
}

int mypipe_register(mypipe_t *pipe, int type)
{
    struct mypipe *p = pipe;

    if (p == NULL || (type != MYPIPE_TYPE_READER && type != MYPIPE_TYPE_WRITER)) {
        return -EINVAL;
    }

    pthread_mutex_lock(&p->mtx);

    if (type & MYPIPE_TYPE_READER) {
        p->reader_count++;
    }
    if (type & MYPIPE_TYPE_WRITER) {
        p->writer_count++;
    }
    pthread_cond_broadcast(&p->cond);

    while (p->reader_count <= 0 || p->writer_count <= 0) {
        pthread_cond_wait(&p->cond, &p->mtx);
    }

    pthread_mutex_unlock(&p->mtx);
    return 0;
}

int mypipe_unregister(mypipe_t *pipe, int type)
{
    struct mypipe *p = pipe;

    if (p == NULL || (type != MYPIPE_TYPE_READER && type != MYPIPE_TYPE_WRITER)) {
        return -EINVAL;
    }

    pthread_mutex_lock(&p->mtx);

    if (type & MYPIPE_TYPE_READER) {
        p->reader_count--;
    }
    if (type & MYPIPE_TYPE_WRITER) {
        p->writer_count--;
    }
    pthread_cond_broadcast(&p->cond);

    pthread_mutex_unlock(&p->mtx);
    return 0;
}

static int mypipe_read_byte_unlocked(struct mypipe *p, char *data)
{
    if (p->size <= 0) {
        return -1;
    }

    *data = p->data[p->head];
    p->head = next(p->head);
    p->size--;
    return 0;
}

int mypipe_read(mypipe_t *pipe, void *buf, size_t count)
{
    size_t i = 0;
    struct mypipe *p = pipe;

    if (p == NULL || buf == NULL) {
        return -EINVAL;
    }

    pthread_mutex_lock(&p->mtx);

    while (p->size <= 0 && p->writer_count > 0) {
        pthread_cond_wait(&p->cond, &p->mtx);
    }

    if (p->size <= 0 && p->writer_count <= 0) {
        pthread_mutex_unlock(&p->mtx);
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (mypipe_read_byte_unlocked(p, buf + i) != 0) {
            break;
        }
    }

    pthread_mutex_unlock(&p->mtx);

    return i;
}

int mypipe_write(mypipe_t *pipe, const void *buf, size_t count)
{
    // 当管道满且有读者才等待

    // 如果管道满且没有读者，则退出

    // 管道满、有读者    --> 等待
    // 管道满、没有读者  --> 退出
    // 管道未满、有读者  --> 可写
    // 管道未满、没有读者 --> 退出
}

int mypipe_destroy(mypipe_t *pipe)
{
    struct mypipe *p = pipe;

    if (p == NULL) {
        return 0;
    }

    pthread_mutex_destroy(p->mtx);
    pthread_cond_destroy(p->cond);
    free(p);
    return 0;
}


