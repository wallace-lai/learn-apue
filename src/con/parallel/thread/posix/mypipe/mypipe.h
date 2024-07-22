#ifndef MYPIPE_H_
#define MYPIPE_H_

#define PIPESIZE                1024
#define MYPIPE_TYPE_READER      0x00000001UL
#define MYPIPE_TYPE_WRITER      0x00000002UL

typedef void mypipe_t;

mypipe_t *mypipe_init(void);

int mypipe_register(mypipe_t *pipe, int type);

int mypipe_unregister(mypipe_t *pipe, int type);

int mypipe_read(mypipe_t *pipe, void *buf, size_t size);

int mypipe_write(mypipe_t *pipe, const void *buf, size_t size);

int mypipe_destroy(mypipe_t *pipe);

#endif /* MYPIPE_H_ */