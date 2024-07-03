#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define THREAD_NUM 20
#define FNAME      "/tmp/out"
#define LINE_SIZE  512

static void *routine(void *ctx)
{
    FILE *f = fopen(FNAME, "r+");
    if (f == NULL) {
        perror("fopen()");
        pthread_exit(NULL);
    }

    char linebuf[LINE_SIZE];
    fgets(linebuf, LINE_SIZE, f);
    int num = atoi(linebuf) + 1;

    fseek(f, 0, SEEK_SET);
    fprintf(f, "%d\n", num);

    fclose(f);
    pthread_exit(NULL);
}

int main()
{
    int err;
    pthread_t tid[THREAD_NUM];

    for (int i = 0; i < THREAD_NUM; i++) {
        err = pthread_create(&tid[i], NULL, routine, NULL);
        if (err) {
            fprintf(stderr, "pthread_create() : %s\n", strerror(err));
            exit(1);
        }
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }

    exit(0);

}