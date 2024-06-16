#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "proto.h"

#define MIN_SPARE_RVER  (5)
#define MAX_SPARE_RVER  (10)
#define MAX_CLIENT_NUM  (20)

#define SIG_NOTIFY      SIGUSR2

enum server_state {
    server_state_idle = 0,
    server_state_busy,
};

struct server {
    pid_t pid;
    int state;
    int reuse;
};

static struct server *server_pool;
static int idle_count = 0;
static int busy_count = 0;

static void usr2_handler(int sig)
{
    return;
}

static void server_job(int sock, int pos)
{
    pid_t ppid = getppid();

    socklen_t raddr_len = 0;
    struct sockaddr_in raddr;

    #define BUFSIZE 64
    static char buffer[BUFSIZ];
    static char ipstr[BUFSIZ];

    while (1) {
        server_pool[pos].state = server_state_idle;
        kill(ppid, SIG_NOTIFY);

        int fd = accept(sock, (void *)&raddr, &raddr_len);
        if (fd < 0) {
            if (errno != EINTR || errno != EAGAIN) {
                perror("accept()");
                exit(1);
            }
        }

        server_pool[pos].state = server_state_busy;
        kill(ppid, SIG_NOTIFY);

        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, BUFSIZ);
        // printf("[%d] Server recv connection from client %s:%d.\n", getpid(), ipstr, ntohs(raddr.sin_port));

        long long stamp = time(NULL);
        int len = snprintf(buffer, BUFSIZ, FORMAT_STAMP, stamp);
        ssize_t ret = send(fd, buffer, len, 0);
        
        sleep(5);
        close(fd);
    }
}

static int add_one_server(int sock)
{
    if (idle_count + busy_count >= MAX_CLIENT_NUM) {
        return -1;
    }

    int slot = 0;
    for (slot = 0; slot < MAX_CLIENT_NUM; slot++) {
        if (server_pool[slot].pid == -1) {
            break;
        }
    }

    server_pool[slot].state = server_state_idle;
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid == 0) {
        // child
        server_job(sock, slot);
        exit(0);
    } else {
        // parent
        server_pool[slot].pid = pid;
        idle_count++;
    }

    return 0;
}

static int del_one_server(void)
{
    if (idle_count == 0) {
        return -1;
    }

    for (int slot = 0; slot < MAX_CLIENT_NUM; slot++) {
        if (server_pool[slot].pid != -1 && server_pool[slot].state == server_state_idle) {
            kill(server_pool[slot].pid, SIGTERM);
            server_pool[slot].pid = -1;
            idle_count--;
            break;
        }
    }

    return 0;
}

static int scan_server_pool()
{
    int idle = 0;
    int busy = 0;

    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
        if (server_pool[i].pid == -1) {
            continue;
        }

        // check if process exist
        if (kill(server_pool[i].pid, 0)) {
            server_pool[i].pid = -1;
            continue;
        }

        if (server_pool[i].state == server_state_idle) {
            idle++;
        } else if (server_pool[i].state == server_state_busy) {
            busy++;
        } else {
            fprintf(stderr, "unknown state");
            abort();
        }
    }

    idle_count = idle;
    busy_count = busy;
    return 0;
}

void show_server_pool_info(void)
{
    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
        if (server_pool[i].pid == -1) {
            putchar(' ');
        } else if (server_pool[i].state == server_state_idle) {
            putchar('.');
        } else {
            putchar('x');
        }
    }
    putchar('\n');
}

int main()
{
    int ret;

    struct sigaction nsa;
    struct sigaction osa;

    nsa.sa_handler = SIG_IGN;
    sigemptyset(&nsa.sa_mask);
    nsa.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &nsa, &osa);

    nsa.sa_handler = usr2_handler;
    sigemptyset(&nsa.sa_mask);
    nsa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &nsa, &osa);

    sigset_t nset;
    sigset_t oset;

    sigemptyset(&nset);
    sigaddset(&nset, SIG_NOTIFY);
    sigprocmask(SIG_BLOCK, &nset, &oset);

    server_pool = mmap(NULL, sizeof(struct server) * MAX_CLIENT_NUM, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (server_pool == MAP_FAILED) {
        perror("mmap()");
        exit(1);
    }
    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
        server_pool[i].pid = -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket()");
        exit(1);
    }

    int val = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(val));
    if (ret < 0) {
        perror("setsockopt()");
        exit(1);
    }

    struct sockaddr_in laddr;
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVER_PORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);

    ret = bind(sock, (void *)&laddr, sizeof(laddr));
    if (ret < 0) {
        perror("bind()");
        exit(1);
    }

    ret = listen(sock, 200);
    if (ret < 0) {
        perror("listen()");
        exit(1);
    }

    for (int i = 0; i < MIN_SPARE_RVER; i++) {
        add_one_server(sock);
    }

    while (1) {
        sigsuspend(&oset);

        scan_server_pool();

        if (idle_count > MAX_SPARE_RVER) {
            for (int i = 0; i < (idle_count - MAX_SPARE_RVER); i++) {
                del_one_server();
            }
        } else if (idle_count < MIN_SPARE_RVER) {
            for (int i = 0; i < (MIN_SPARE_RVER - idle_count); i++) {
                add_one_server(sock);
            }
        }

        show_server_pool_info();
    }

    sigprocmask(SIG_SETMASK, &oset, NULL);
}