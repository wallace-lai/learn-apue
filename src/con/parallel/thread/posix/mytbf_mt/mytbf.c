#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "mytbf.h"

#define MAX_MYTBF_SIZE (1024)

struct mytbf_s {
    pthread_mutex_t mtx;
    unsigned cps;
    unsigned burst;
    unsigned token;
    int pos;
};

static pthread_t worker;

static struct mytbf_s *job[MAX_MYTBF_SIZE];
static pthread_mutex_t job_mtx = PTHREAD_MUTEX_INITIALIZER;

static pthread_once_t once_control = PTHREAD_ONCE_INIT;

static int get_free_pos_unlocked()
{
    for (int i = 0; i < MAX_MYTBF_SIZE; i++) {
        if (job[i] == NULL) {
            return i;
        }
    }

    return -1;
}

static int min(int a, int b)
{
    return (a < b) ? a : b;
}

mytbf_t *mytbf_init(unsigned cps, unsigned burst)
{
    int pos = -1;
    struct mytbf_s *tbf = NULL;

    tbf = malloc(sizeof(struct mytbf_s));
    if (tbf == NULL) {
        return NULL;
    }
    pthread_mutex_init(&tbf->mtx, NULL);

    pthread_mutex_lock(&job_mtx);
    pos = get_free_pos_unlocked();
    if (pos < 0) {
        pthread_mutex_unlock(&job_mtx);
        pthread_mutex_destroy(&tbf->mtx);
        free(tbf);
        return NULL;
    }

    tbf->cps = cps;
    tbf->burst = burst;
    tbf->token = 0;
    tbf->pos = pos;
    job[pos] = tbf;
    pthread_mutex_unlock(&job_mtx);

    return tbf;
}

int mytbf_fetch_token(mytbf_t *tbf, int num)
{
    int n = 0;
    struct mytbf_s *t = NULL;

    if (num <= 0) {
        return -EINVAL;
    }

    t = (struct mytbf_s *)tbf;
    pthread_mutex_lock(&t->mtx);

    while (t->token <= 0) {
        pthread_mutex_unlock(&t->mtx);
        sched_yield();
        pthread_mutex_lock(&t->mtx);
    }
    n = min(t->token, num);
    t->token -= n;

    pthread_mutex_unlock(&t->mtx);

    return n;
}

int mytbf_return_token(mytbf_t *tbf, int num)
{
    struct mytbf_s *t = NULL;

    if (num <= 0) {
        return -EINVAL;
    }

    t = (struct mytbf_s *)tbf;
    pthread_mutex_lock(&t->mtx);
    t->token += num;
    if (t->token > t->burst) {
        t->token = t->burst;
    }
    pthread_mutex_unlock(&t->mtx);

    return num;
}

int mytbf_deinit(mytbf_t *tbf)
{
    struct mytbf_s *t = NULL;
    
    t = (struct mytbf_s *)tbf;
    pthread_mutex_lock(&job_mtx);
    job[t->pos] = NULL;
    pthread_mutex_unlock(&job_mtx);

    pthread_mutex_destroy(&t->mtx);
    free(t);

    return 0;
}

static void *worker_routine(void *ctx)
{
    int ret = 0;
    struct timespec req;
    struct timespec rem;

    while (1) {
        // 计时1秒
        req.tv_sec = 1;
        req.tv_nsec = 0;
        while (1) {
            ret = nanosleep(&req, &rem);
            if (ret == 0) {
                break;
            } else if (ret < 0 && errno == EINTR) {
                if (rem.tv_sec > 0 || rem.tv_nsec > 0) {
                    req = rem;
                    continue;
                }
            } else {
                perror("nanosleep()");
                pthread_exit(NULL);
            }
        }

        // 加token
        pthread_mutex_lock(&job_mtx);
        for (int i = 0; i < MAX_MYTBF_SIZE; i++) {
            if (job[i] != NULL) {
                pthread_mutex_lock(&job[i]->mtx);
                job[i]->token += job[i]->cps;
                if (job[i]->token > job[i]->burst) {
                    job[i]->token = job[i]->burst;
                }
                pthread_mutex_unlock(&job[i]->mtx);
            }
        }
        pthread_mutex_unlock(&job_mtx);
    }

    pthread_exit(NULL);
}

static void module_load_impl(void)
{
    int err = pthread_create(&worker, NULL, worker_routine, NULL);
    if (err) {
        fprintf(stderr, "pthread_create() : %s\n", strerror(err));
        pthread_exit(NULL);
    }
}

void mytbf_module_load()
{
    (void)pthread_once(&once_control, module_load_impl);
}

void mytbf_module_unload()
{
    pthread_cancel(worker);
    pthread_join(worker, NULL);
}