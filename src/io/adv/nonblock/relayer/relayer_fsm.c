#include <errno.h>
#include <stdlib.h>

#include "relayer_fsm.h"

static void fsm_on_state_r(relayer_fsm_t *fsm)
{
    fsm->len = read(fsm->sfd, fsm->buffer, BUFSIZE);
    if (fsm->len == 0) {
        fsm->state = FSM_STATE_T;
    } else if (fsm->len < 0) {
        if (errno == EAGAIN) {
            fsm->state = FSM_STATE_R;
        } else {
            fsm->errmsg = "read()";
            fsm->state = FSM_STATE_EX;
        }
    } else {
        fsm->pos = 0;
        fsm->state = FSM_STATE_W;
    }
}

static void fsm_on_state_w(relayer_fsm_t *fsm)
{
    int ret;

    ret = write(fsm->dfd, fsm->buffer + fsm->pos, fsm->len);
    if (ret < 0) {
        if (errno == EAGAIN) {
            fsm->state = FSM_STATE_W;
        } else {
            fsm->errmsg = "write()";
            fsm->state = FSM_STATE_EX;
        }
    } else {
        fsm->len -= ret;
        fsm->pos += ret;
        if (fsm->len == 0) {
            fsm->state = FSM_STATE_R;
        } else {
            fsm->state = FSM_STATE_W;
        }
    }
}

static void fsm_on_state_ex(relayer_fsm_t *fsm)
{
    perror(fsm->errmsg);
    fsm->state = FSM_STATE_T;
}

static void fsm_on_state_t(relayer_fsm_t *fsm)
{
    /* do something */
}

void fsm_driver(relayer_fsm_t *fsm)
{
    if (fsm == NULL) {
        return;
    }

    switch (fsm->state) {
        case FSM_STATE_R:
            fsm_on_state_r(fsm);
            break;
        case FSM_STATE_W:
            fsm_on_state_w(fsm);
            break;
        case FSM_STATE_EX:
            fsm_on_state_ex(fsm);
            break;
        case FSM_STATE_T:
            fsm_on_state_t(fsm);
            break;
        default:
            abort();
            break;
    }
}

void relayer_fsm_init(relayer_fsm_t *fsm, int sfd, int dfd)
{
    if (fsm == NULL) {
        return;
    }

    memset(fsm, 0, sizeof(*fsm));
    fsm->state = FSM_STATE_R;
    fsm->sfd = sfd;
    fsm->dfd = dfd;
}