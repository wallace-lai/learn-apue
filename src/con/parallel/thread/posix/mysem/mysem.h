#ifndef MYSEM_H_
#define MYSEM_H_

typedef void mysem_t;

mysem_t *mysem_init(int value);

int mysem_add(mysem_t *sem, int num);

int mysem_sub(mysem_t *sem, int num);

int mysem_destroy(mysem_t *sem);

#endif /* MYSEM_H_ */