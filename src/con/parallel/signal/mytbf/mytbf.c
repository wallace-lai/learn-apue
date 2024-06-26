#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "mytbf.h"

#define MAX_MYTBF_SIZE (1024)

struct mytbf_s {
    unsigned cps;
    unsigned burst;
    unsigned token;
    int pos;
};

typedef void (*sighandler_t)(int);

static struct mytbf_s *job[MAX_MYTBF_SIZE];
static int mytbf_module_loaded = 0;
static sighandler_t alarm_handler_save;

static int get_free_pos()
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

static void on_alarm_handler(int s)
{
    // set next alarm signal
    alarm(1);

    // grant tokens to every job
    for (int i = 0; i < MAX_MYTBF_SIZE; i++) {
        if (job[i] != NULL) {
            job[i]->token += job[i]->cps;
            if (job[i]->token > job[i]->burst) {
                job[i]->token = job[i]->burst;
            }
        }
    }
}

static void mytbf_module_unload();
static void mytbf_module_load()
{
    if (mytbf_module_loaded) {
        return;
    }

    alarm_handler_save = signal(SIGALRM, on_alarm_handler);
    alarm(1);
    mytbf_module_loaded = 1;

    atexit(mytbf_module_unload);
}

static void mytbf_module_unload()
{
    if (!mytbf_module_loaded) {
        return;
    }

    signal(SIGALRM, alarm_handler_save);
    alarm(0);

    for (int i = 0; i < MAX_MYTBF_SIZE; i++) {
        free(job[i]);
    }
}

mytbf_t *mytbf_init(unsigned cps, unsigned burst)
{
    mytbf_module_load();

    int pos = get_free_pos();
    if (pos < 0) {
        return NULL;
    }

    struct mytbf_s *tbf = malloc(sizeof(struct mytbf_s));
    if (tbf == NULL) {
        return NULL;
    }

    tbf->cps = cps;
    tbf->burst = burst;
    tbf->token = 0;
    tbf->pos = pos;
    job[pos] = tbf;

    return tbf;
}

int mytbf_fetch_token(mytbf_t *tbf, int num)
{
    if (num <= 0) {
        return -EINVAL;
    }

    struct mytbf_s *t = (struct mytbf_s *)tbf;
    while (t->token <= 0) {
        pause();
    }

    int n = min(t->token, num);
    t->token -= n;
    return n;
}

int mytbf_return_token(mytbf_t *tbf, int num)
{
    if (num <= 0) {
        return -EINVAL;
    }

    struct mytbf_s *t = (struct mytbf_s *)tbf;
    t->token += num;
    if (t->token > t->burst) {
        t->token = t->burst;
    }

    return num;
}

int mytbf_deinit(mytbf_t *tbf)
{
    struct mytbf_s *t = (struct mytbf_s *)tbf;
    job[t->pos] = NULL;
    free(t);

    return 0;
}