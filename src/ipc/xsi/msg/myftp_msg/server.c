// server.c

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"

typedef enum server_state_e {
    server_state_start,
    server_state_wait_path,
    server_state_send_data,
    server_state_send_eot,
    server_state_fail,
    server_state_end,
} server_state_t;

typedef struct file_reader_s {
    FILE *handler;
    long offset;
} file_reader_t;

typedef struct server_context_s {
    server_state_t state;
    int msgqid;
    file_reader_t reader;
    msg_s2c_t send_msg;
    msg_c2s_t recv_msg;
} server_context_t;

int main()
{
    int ret;
    ssize_t len;

    key_t key = ftok(KEYPATH, KEYPROJ);
    if (key < 0) {
        perror("ftok()");
        exit(1);
    }

    // message queue created by the server
    int msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid < 0) {
        perror("msgget()");
        exit(1);
    }

    server_context_t ctx = {
        .state = server_state_start,
        .msgqid = msgid,
    };
    memset(&ctx.reader, 0, sizeof(file_reader_t));
    memset(&ctx.send_msg, 0, sizeof(msg_s2c_t));
    memset(&ctx.recv_msg, 0, sizeof(msg_c2s_t));

    while (ctx.state != server_state_end) {
        switch (ctx.state) {
            case server_state_start:
                ctx.state = server_state_wait_path;
                break;

            case server_state_wait_path:
                len = msgrcv(ctx.msgqid, &ctx.recv_msg, sizeof(msg_c2s_t), 0, 0);
                if (len < 0) {
                    ctx.state = server_state_fail;
                } else {
                    assert(ctx.recv_msg.mtype == MSG_TYPE_PATH);
                    assert(ctx.reader.handler == NULL);

                    FILE *f = fopen(ctx.recv_msg.msg_path.path, "rb");
                    if (f == NULL) {    // file not found
                        ctx.state = server_state_fail;
                    } else {
                        ctx.reader.handler = f;
                        ctx.reader.offset = 0;
                        ctx.state = server_state_send_data;
                    }
                }
                break;

            case server_state_send_data:
                assert(ctx.reader.handler != NULL);

                // we need to set offset before read
                if (fseek(ctx.reader.handler, ctx.reader.offset, SEEK_SET) == 0) {
                    #define BUF_SIZE 10
                    static char buffer[BUF_SIZE];
                    memset(buffer, 0, BUF_SIZE);

                    size_t bytes_read = fread(buffer, sizeof(char), BUF_SIZE, ctx.reader.handler);
                    if (bytes_read > 0) {
                        memset(&ctx.send_msg, 0, sizeof(msg_s2c_t));
                        ctx.send_msg.mtype = MSG_TYPE_DATA;
                        memcpy(&ctx.send_msg.msg_data.data, buffer, BUF_SIZE);

                        ret = msgsnd(ctx.msgqid, &ctx.send_msg, sizeof(msg_s2c_t), 0);
                        if (ret < 0) {
                            ctx.state = server_state_fail;
                        } else {
                            ctx.reader.offset += bytes_read;
                            ctx.state = server_state_send_data;
                        }
                    } else {
                        // we also treat read error as the end of transations
                        ctx.state = server_state_send_eot;
                    }
                } else {
                    ctx.state = server_state_fail;
                }
                break;

            case server_state_send_eot:
                memset(&ctx.send_msg, 0, sizeof(msg_s2c_t));
                ctx.send_msg.mtype = MSG_TYPE_EOT;
                ret = msgsnd(ctx.msgqid, &ctx.send_msg, sizeof(msg_s2c_t), 0);
                if (ret < 0) {
                    ctx.state = server_state_fail;
                } else {
                    // release resources for end state
                    if (ctx.reader.handler != NULL) {
                        fclose(ctx.reader.handler);
                    }
                    ctx.state = server_state_end;
                }
                break;

            case server_state_fail:
                fprintf(stdout, "send date to client failed for unknown reaseon.");
                if (ctx.reader.handler != NULL) {
                    fclose(ctx.reader.handler);
                }
                ctx.state = server_state_end;
                break;

            default:
                ctx.state = server_state_end;
                break;
        }
    }

    return 0;
}