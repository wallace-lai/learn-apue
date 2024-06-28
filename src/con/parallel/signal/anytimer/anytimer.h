// anytimer.h

#ifndef ANYTIMER_H_
#define ANYTIMER_H_

typedef void (*anytimer_func_t)(void *);

/**
 * RETURN VALUE
 *     >= 0         成功，返回任务id
 *     == -EINVAL   失败，参数非法
 *     == -ENOSPC   失败，数组满
 *     == -ENOMEM   失败，内存不足
 */
int anytimer_add_job(unsigned sec, anytimer_func_t job, void *arg);

/**
 * RETURN VALUE
 *     == 0             成功，指定任务已取消
 *     == -EINVAL       失败，参数非法
 *     == -EBUSY        失败，指定任务已完成
 *     == -ECANCELED    失败，重复取消指定任务
 */
int anytimer_cancel_job(int id);

/**
 * RETURN VALUE
 *     == 0             成功，指定任务成功释放
 *     == -EINVAL       失败，参数非法
 */
int anytimer_wait_job(int id);

int anytimer_pause_job(int id);

int anytimer_resume_job(int id);

#endif /* ANYTIMER_H_ */