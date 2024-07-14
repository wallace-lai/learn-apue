#include "relayer.h"
#include "relayer_fsm.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>

#define RELAYER_JOBMAX (10000)

enum {
    JOB_STATE_RUNNING = 1,
    JOB_STATE_CANCELED,
    JOB_STATE_OVER,
};

typedef struct relayer_job_s {
    int job_state;
    int fd1;
    int fd2;

    relayer_fsm_t m12;
    relayer_fsm_t m21;

    int fd1_save;
    int fd2_save;
} relayer_job_t;

static relayer_job_t *g_relayers[RELAYER_JOBMAX];
static pthread_mutex_t relayers_mutex = PTHREAD_MUTEX_INITIALIZER;

static int get_free_pos_unlocked(void)
{
    for (int i = 0; i < RELAYER_JOBMAX; i++) {
        if (g_relayers[i] == NULL) {
            return i;
        }
    }

    return -1;
}

void *relayer_routine(void *arg)
{
    relayer_job_t *job = NULL;

    while (1) {
        pthread_mutex_lock(&relayers_mutex);
        for (int i = 0; i < RELAYER_JOBMAX; i++) {
            job = g_relayers[i];
            if (job == NULL || job->job_state == STATE_CANCELED) {
                continue;
            }

            if (job->job_state == STATE_RUNNING) {
                fsm_driver(&job->m12);
                fsm_driver(&job->m21);
                if (job->m12.state == FSM_STATE_T && job->m21.state == FSM_STATE_T) {
                    job->job_state = STATE_OVER;
                }
            }
        }
        pthread_mutex_unlock(&relayers_mutex);
    }
}

/**
 * RETURN VALUE
 *     >= 0    成功，返回当前任务ID号
 *     -EINVAL 失败，参数非法
 *     -ENOSPC 失败，任务数组已满
 *     -ENOMEM 失败，内存分配有误
 */
int relayer_add_job(int fd1, int fd2)
{
    relayer_job_t *job = NULL;

    job = malloc(sizeof(*job));
    if (job == NULL) {
        return -ENOMEM;
    }

    job->fd1 = fd1;
    job->fd2 = fd2;
    job->job_state = JOB_STATE_RUNNING;

    job->fd1_save = fcntl(job->fd1, F_GETFL);
    fcntl(job->fd1, F_SETFL, job->fd1_save | O_NONBLOCK);
    job->fd2_save = fcntl(job->fd2, F_GETFL);
    fcntl(job->fd2, F_SETFL, job->fd2_save | O_NONBLOCK);

    relayer_fsm_init(&job->m12, fd1, fd2);
    relayer_fsm_init(&job->m21, fd2, fd1);

    int pos = -1;
    pthread_mutex_lock(&relayers_mutex);
    pos = get_free_pos_unlocked();
    if (pos < 0) {
        pthread_mutex_unlock(&relayers_mutex);
        fcntl(job->fd1, F_SETFL, job->fd1_save);
        fcntl(job->fd2, F_SETFL, job->fd2_save);
        free(job);
        return -ENOSPC;
    }
    g_relayers[pos] = job;
    pthread_mutex_unlock(&relayers_mutex);

    return pos;
}

/**
 * RETURN VALUE
 *     == 0    成功，指定任务已经成功取消
 *     -EINVAL 失败，参数非法
 *     -EBUSY  失败，任务重复取消
 */
int relayer_cancel_job(int id)
{
    if (id < 0 || id >= RELAYER_JOBMAX) {
        return -EINVAL;
    }

    relayer_job_t *job = g_relayers[id];

    pthread_mutex_lock(&relayers_mutex);
    while (job->job_state != STATE_OVER) {
        pthread_mutex_unlock(&relayers_mutex);
        sched_yield();
        pthread_mutex_lock(&relayers_mutex);
    }

    job->job_state = STATE_CANCELED;
    pthread_mutex_unlock(&relayers_mutex);
    return 0;
}

static void copy_job_stat(relayer_job_t *job, relayer_stat_t *stat)
{
    stat->fd1 = job->fd1;
    stat->fd2 = job->fd2;
    stat->state = job->job_state;
    stat->count12 = job->m12.count;
    stat->count21 = job->m21.count;
}

/**
 * RETURN VALUE
 *     == 0        成功，指定任务已经终止并返回状态
 *     == -EINVAL  失败，参数非法
 */
int relayer_wait_job(int id, relayer_stat_t *stat)
{
    if (id < 0 || id >= RELAYER_JOBMAX) {
        return -EINVAL;
    }

    relayer_job_t *job = g_relayers[id];

    pthread_mutex_lock(&relayers_mutex);
    if (job->job_state == STATE_CANCELED ||
        job->job_state == STATE_OVER) {
        copy_job_stat(job, stat);
        pthread_mutex_unlock(&relayers_mutex);
        return 0;
    }

    // job is running
    while (job->job_state != STATE_OVER) {
        pthread_mutex_unlock(&relayers_mutex);
        sched_yield();
        pthread_mutex_lock(&relayers_mutex);
    }

    copy_job_stat(job, stat);
    pthread_mutex_unlock(&relayers_mutex);
    return 0;
}

/**
 * RETURN VALUE
 *     == 0        成功，指定任务状态已返回
 *     == -EINVAL  失败，参数非法
 */
int relayer_get_state(int id, relayer_stat_t *stat)
{
    if (id < 0 || id >= RELAYER_JOBMAX) {
        return -EINVAL;
    }

    relayer_job_t *job = g_relayers[id];
    pthread_mutex_lock(&relayers_mutex);
    copy_job_stat(job, stat);
    pthread_mutex_unlock(&relayers_mutex);
    return 0;
}