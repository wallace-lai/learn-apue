#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "proto.h"

#define IPSTRSIZE 32

int main()
{
    int ret;

    int sock = socket(AF_INET, SOCK_DGRAM, 0 /* IPPROTO_UDP */);
    if (sock < 0) {
        perror("socket()");
        exit(1);
    }

    // 打开广播标志
    int val = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&val, sizeof(val));
    if (ret < 0) {
        perror("setsockopt()");
        exit(1);
    }

    struct sockaddr_in laddr;
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RECVPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr);

    ret = bind(sock, (void *)&laddr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        perror("bind()");
        exit(1);
    }

    
    static char ipstr[IPSTRSIZE];
    struct msg_s buffer;
    struct sockaddr_in raddr;
    socklen_t rlen = 0;
    while (1) {
        recvfrom(sock, &buffer, sizeof(buffer), 0, (void *)&raddr, &rlen);

        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
        printf("--- RECV MESSAGE FROM %s:%d ---\n", ipstr, ntohs(raddr.sin_port));
        printf("NAME : %s\n", buffer.name);
        printf("MATH : %d\n", ntohl(buffer.math));
        printf("CHINESE : %d\n", ntohl(buffer.chinese));
    }

    close(sock);
    exit(0);
}