#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#include "mytbf.h"

#define MAX_MYTBF_SIZE (1024)

struct mytbf_s {
    unsigned cps;
    unsigned burst;
    unsigned token;
    int pos;
};

static struct mytbf_s *job[MAX_MYTBF_SIZE];
static int mytbf_module_loaded = 0;
static struct sigaction alarm_sa_save;

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

// static void on_alarm_handler(int s)
static void on_alarm_action(int s, siginfo_t *infop, void *unused)
{
    // set next alarm signal
    // alarm(1);

    if (infop->si_code != SI_KERNEL) {
        // ignore SIGALRM from user space
        return;
    }

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
    int ret;

    if (mytbf_module_loaded) {
        return;
    }

    // alarm_handler_save = signal(SIGALRM, on_alarm_handler);
    // alarm(1);

    struct sigaction sa;
    sa.sa_sigaction = on_alarm_action;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    ret = sigaction(SIGALRM, &sa, &alarm_sa_save);
    if (ret < 0) {
        perror("sigaction()");
        exit(1);
    }

    struct itimerval itv;
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;

    ret = setitimer(ITIMER_REAL, &itv, NULL);
    if (ret < 0) {
        perror("setitimer()");
        exit(1);
    }

    atexit(mytbf_module_unload);
    mytbf_module_loaded = 1;
}

static void mytbf_module_unload()
{
    if (!mytbf_module_loaded) {
        return;
    }

    // signal(SIGALRM, alarm_handler_save);
    // alarm(0);

    sigaction(SIGALRM, &alarm_sa_save, NULL);

    struct itimerval itv;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_interval.tv_sec = 0;
    itv.it_value.tv_usec = 0;
    (void)setitimer(ITIMER_REAL, &itv, NULL);

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