#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "proto.h"

#define IPSTRSIZE 64
#define STAMPSIZE 128

static void server_execute(int fd)
{
    static char buffer[STAMPSIZE];
    memset(buffer, 0, STAMPSIZE);

    int len = sprintf(buffer, FORMAT_STAMP, (long long)time(NULL));
    int ret = send(fd, buffer, len, 0);
    if (ret < 0) {
        perror("send()");
        exit(1);
    }
}

int main()
{
    int ret;

    int sock = socket(AF_INET, SOCK_STREAM, 0 /* IPPROTO_TCP, IPPROTO_SCTP */);
    if (sock < 0) {
        perror("socket()");
        exit(1);
    }

    int val = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (ret < 0) {
        perror("setsockopt()");
        exit(1);
    }

    struct sockaddr_in laddr;
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVER_PORT));
    inet_pton(AF_INET, "0.0.0.0", (void *)&laddr.sin_addr);

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

    struct sockaddr_in raddr;
    socklen_t raddr_len = 0;
    static char ipstr[IPSTRSIZE];
    while (1) {
        ret = accept(sock, (void *)&raddr, &raddr_len);
        if (ret < 0) {
            perror("accept()");
            exit(1);
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(1);
        }

        if (pid == 0) {
            // child
            memset(ipstr, 0, IPSTRSIZE);
            inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
            printf("Recv connection from client (%s:%d).\n", ipstr, ntohs(raddr.sin_port));

            server_execute(ret);
            close(ret);
            exit(0);
        }
    }

    close(sock);
    exit(0);
    
}