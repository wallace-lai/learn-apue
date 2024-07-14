#ifndef RELAYER_FSM_H_
#define RELAYER_FSM_H_

#include <stdint.h>

#define BUFSIZE (1024)

enum {
    FSM_STATE_R = 1,
    FSM_STATE_W,
    FSM_STATE_EX,
    FSM_STATE_T,
};

typedef struct relayer_fsm_s {
    int state;
    int sfd;
    int dfd;

    // FSM_STATE_R FSM_STATE_W
    int len;
    int pos;
    char buffer[BUFSIZE];

    // FSM_STATE_EX
    char *errmsg;

    // STAT
    uint64_t count;
} relayer_fsm_t;

void relayer_fsm_init(relayer_fsm_t *fsm, int sfd, int dfd);
void fsm_driver(relayer_fsm_t *fsm);

#endif /* RELAYER_FSM_H_ */