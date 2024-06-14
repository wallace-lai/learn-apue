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
#define BUFSIZE 1024

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
    raddr.sin_port = htons(80);
    inet_pton(AF_INET, argv[1], (void *)&raddr.sin_addr);

    ret = connect(sock, (void *)&raddr, sizeof(raddr));
    if (ret < 0) {
        perror("connect()");
        exit(1);
    }

    // open socket file descriptor
    FILE *fp = fdopen(sock, "r+");
    if (fp == NULL) {
        perror("fdopen()");
        exit(1);
    }

    // write http request to socket
    fprintf(fp, "GET /logo.jpg\r\n\r\n");
    fflush(fp);

    // recv response data from socket
    size_t len = 0;
    static char buffer[BUFSIZ];
    while (1) {
        len = fread(buffer, 1, BUFSIZ, fp);
        if (len <= 0) {
            break;
        }

        fwrite(buffer, 1, len, stdout);
    }


    fclose(fp);
    close(sock);
    exit(0);
}