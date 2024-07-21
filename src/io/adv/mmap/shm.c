#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MEMSIZE 1024

int main()
{
    char *ptr = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap()");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork()");
        munmap(ptr, MEMSIZE);
        exit(1);
    }

    if (pid == 0) {
        // child write
        strncpy(ptr, "hello message form child process.", MEMSIZE);
        munmap(ptr, MEMSIZE);
        exit(0);
    } else {
        // parent read
        wait(NULL);
        puts(ptr);
        munmap(ptr, MEMSIZE);
    }

    exit(0);
}