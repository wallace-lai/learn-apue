// client.c

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"

typedef enum client_state_e {
    client_state_start,
    client_state_send_path,
    client_state_wait_data,
    client_state_recv_data,
    client_state_recv_eot,
    client_state_fail,
    client_state_end,
} client_state_t;

typedef struct client_context_s {
    client_state_t state;
    int msgqid;
    msg_c2s_t send_msg;
    msg_s2c_t recv_msg;
} client_context_t;

#define FILEPATH "/home/lzh/dev/learn-apue/src/ipc/xsi/msg/myftp_msg/data.txt"

int main()
{
    int ret;

    key_t key = ftok(KEYPATH, KEYPROJ);
    if (key < 0) {
        perror("ftok()");
        exit(1);
    }

    // message queue should created by the server not the client
    // so you have to start client process after server process is started
    int msgid = msgget(key, 0);
    if (msgid < 0) {
        perror("msgget()");
        exit(1);
    }

    client_context_t ctx = {
        .state = client_state_start,
        .msgqid = msgid,
    };
    memset(&ctx.send_msg, 0, sizeof(msg_c2s_t));
    memset(&ctx.recv_msg, 0, sizeof(msg_s2c_t));

    while (ctx.state != client_state_end) {
        switch (ctx.state) {
            case client_state_start:
                ctx.send_msg.mtype = MSG_TYPE_PATH;
                strncpy(ctx.send_msg.msg_path.path, FILEPATH, PATHMAX);
                ctx.state = client_state_send_path;
                break;

            case client_state_send_path:
                ret = msgsnd(ctx.msgqid, &ctx.send_msg, sizeof(msg_c2s_t), 0);
                if (ret < 0) {
                    ctx.state = client_state_fail;
                } else {
                    ctx.state = client_state_wait_data;
                }
                break;

            case client_state_wait_data:
                // think about why we need to memset recv_msg here again
                memset(&ctx.recv_msg, 0, sizeof(msg_s2c_t));

                ssize_t len = msgrcv(ctx.msgqid, &ctx.recv_msg, sizeof(msg_s2c_t), 0, 0);
                if (len < 0) {
                    ctx.state = client_state_fail;
                } else {
                    assert(ctx.recv_msg.mtype == MSG_TYPE_DATA || ctx.recv_msg.mtype == MSG_TYPE_EOT);
                    if (ctx.recv_msg.mtype == MSG_TYPE_DATA) {
                        ctx.state = client_state_recv_data;
                    } else {
                        ctx.state = client_state_recv_eot;
                    }
                }
                break;

            case client_state_recv_data:
                assert(ctx.recv_msg.mtype == MSG_TYPE_DATA);
                msg_data_t *recv_data = &(ctx.recv_msg.msg_data);
                fprintf(stdout, "recv data :\n%s\n", recv_data->data);
                ctx.state = client_state_wait_data;
                break;

            case client_state_recv_eot:
                assert(ctx.recv_msg.mtype == MSG_TYPE_EOT);
                fprintf(stdout, "recv data end.\n");
                ctx.state = client_state_end;
                break;

            case client_state_fail:
                fprintf(stdout, "receive data from server failed for unknown reason.");
                ctx.state = client_state_end;
                break;

            default:
                // unreachable branch
                ctx.state = client_state_end;
                break;
        }
    }

    return 0;
}