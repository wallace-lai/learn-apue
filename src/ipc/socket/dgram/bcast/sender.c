#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "proto.h"

int main(int argc, char **argv)
{
    int ret;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
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

    // bind();

    srand(time(NULL));

    struct msg_s buffer;
    strncpy(buffer.name, "Alice", NAMESIZE);
    buffer.math = htonl(rand() % 100);
    buffer.chinese = htonl(rand() % 100);

    struct sockaddr_in raddr;
    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(RECVPORT));
    inet_pton(AF_INET, "255.255.255.255", &raddr.sin_addr.s_addr);

    ret = sendto(sock, &buffer, sizeof(buffer), 0, (void *)&raddr, sizeof(raddr));
    if (ret < 0) {
        perror("sendto()");
        exit(1);
    }
    puts("Send OK.");

    close(sock);
    exit(0);
}