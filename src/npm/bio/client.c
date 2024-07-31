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

#define STAMPSIZE 128

int main(int argc, char **argv)
{
    int ret;

    if (argc < 2) {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket()");
        exit(1);
    }

    // bind();

    struct sockaddr_in raddr;
    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(SERVER_PORT));
    inet_pton(AF_INET, argv[1], (void *)&raddr.sin_addr);

    ret = connect(sock, (void *)&raddr, sizeof(raddr));
    if (ret < 0) {
        perror("connect()");
        exit(1);
    }

    static char buffer[STAMPSIZE];
    memset(buffer, 0, STAMPSIZE);

    FILE *fp = fdopen(sock, "r");
    if (fp == NULL) {
        perror("fdopen()");
        exit(1);
    }
    if (fgets(buffer, STAMPSIZE, fp) != NULL) {
        printf("%s\r\n", buffer);
    }
    fclose(fp);

    close(sock);
    exit(0);
}