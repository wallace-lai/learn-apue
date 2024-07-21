#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"

#define BUFSIZE (1024)

enum {
    FSM_STATE_R = 1,
    FSM_STATE_W,
    FSM_STATE_EX,
    FSM_STATE_T,
};

typedef struct fsm_s {
    int state;
    int sfd;
    int dfd;

    // FSM_STATE_R FSM_STATE_W
    int len;
    int pos;
    char buffer[BUFSIZ];

    // FSM_STATE_EX
    char *errmsg;
} fsm_t;

static void fsm_on_state_r(fsm_t *fsm)
{
    fsm->len = read(fsm->sfd, fsm->buffer, BUFSIZ);
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

static void fsm_on_state_w(fsm_t *fsm)
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

static void fsm_on_state_ex(fsm_t *fsm)
{
    perror(fsm->errmsg);
    fsm->state = FSM_STATE_T;
}

static void fsm_on_state_t(fsm_t *fsm)
{
    /* do something */
}

void fsm_driver(fsm_t *fsm)
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

static inline int max(int a, int b)
{
    return (a > b) ? a : b;
}

int relay(int sfd, int dfd)
{
    int ret;
    int sfd_save;
    int dfd_save;

    sfd_save = fcntl(sfd, F_GETFL);
    fcntl(sfd, F_SETFL, sfd_save | O_NONBLOCK);

    dfd_save = fcntl(dfd, F_GETFL);
    fcntl(dfd, F_SETFL, dfd_save | O_NONBLOCK);

    fsm_t m12;
    fsm_t m21;

    m12.state = FSM_STATE_R;
    m12.sfd = sfd;
    m12.dfd = dfd;

    m21.state = FSM_STATE_R;
    m21.sfd = dfd;
    m21.dfd = sfd;

    fd_set rset;
    fd_set wset;

    while (m12.state != FSM_STATE_T || m21.state != FSM_STATE_T) {
        // FSM_STATE_EX态需要自动往下推
        if (m12.state == FSM_STATE_EX || m21.state == FSM_STATE_EX) {
            fsm_driver(&m12);
            fsm_driver(&m21);
            continue;
        }

        // 布置监视任务
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        if (m12.state == FSM_STATE_R) {
            FD_SET(m12.sfd, &rset);
        } else if (m12.state == FSM_STATE_W) {
            FD_SET(m12.dfd, &wset);
        }

        if (m21.state == FSM_STATE_R) {
            FD_SET(m21.sfd, &rset);
        } else if (m21.state == FSM_STATE_W) {
            FD_SET(m21.dfd, &wset);
        }

        // 监视
        ret = select(max(sfd, dfd) + 1, &rset, &wset, NULL, NULL);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select()");
            exit(1);
        }

        // 查看监视结果
        if (FD_ISSET(m12.sfd, &rset) || FD_ISSET(m12.dfd, &wset)) {
            fsm_driver(&m12);
        }
        if (FD_ISSET(m21.sfd, &rset) || FD_ISSET(m21.dfd, &wset)) {
            fsm_driver(&m21);
        }
    }

    fcntl(sfd, F_SETFL, sfd_save);
    fcntl(dfd, F_SETFL, dfd_save);
}

int main(int argc, char **argv)
{
    int sfd = open(TTY1, O_RDWR);
    if (sfd < 0) {
        perror("open()");
        exit(1);
    }
    write(sfd, "TTY1\n", 5);

    int dfd = open(TTY2, O_RDWR);
    if (dfd < 0) {
        perror("open()");
        exit(1);
    }
    write(dfd, "TTY2\n", 5);

    relay(sfd, dfd);

    close(dfd);
    close(sfd);
    exit(0);
}