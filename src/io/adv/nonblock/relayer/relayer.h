#ifndef RELAYER_H_
#define RELAYER_H_

#include <stdint.h>

enum {
    STATE_RUNNING = 1,
    STATE_CANCELED,
    STATE_OVER,
};

typedef struct relayer_stat_s {
    int state;
    int fd1;
    int fd2;

    uint64_t count12;
    uint64_t count21;
    // struct timerval start;
    // struct timerval end;
} relayer_stat_t;

void *relayer_routine(void *arg);

/**
 * RETURN VALUE
 *     >= 0    成功，返回当前任务ID号
 *     -EINVAL 失败，参数非法
 *     -ENOSPC 失败，任务数组已满
 *     -ENOMEM 失败，内存分配有误
 */
int relayer_add_job(int fd1, int fd2);

/**
 * RETURN VALUE
 *     == 0    成功，指定任务已经成功取消
 *     -EINVAL 失败，参数非法
 *     -EBUSY  失败，任务重复取消
 */
int relayer_cancel_job(int id);

/**
 * RETURN VALUE
 *     == 0        成功，指定任务已经终止并返回状态
 *     == -EINVAL  失败，参数非法
 */
int relayer_wait_job(int id, relayer_stat_t *stat);

/**
 * RETURN VALUE
 *     == 0        成功，指定任务状态已返回
 *     == -EINVAL  失败，参数非法
 */
int relayer_get_stat(int id, relayer_stat_t *stat);

#endif /* RELAYER_H_ */