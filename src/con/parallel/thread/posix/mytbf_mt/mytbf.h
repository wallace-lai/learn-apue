// mytbf.h

#ifndef MY_TBF_H_
#define MY_TBF_H_

typedef void mytbf_t;

mytbf_t *mytbf_init(unsigned cps, unsigned burst);

int mytbf_fetch_token(mytbf_t *tbf, int num);

int mytbf_return_token(mytbf_t *tbf, int num);

int mytbf_deinit(mytbf_t *tbf);

#endif /* MY_TBF_H_ */