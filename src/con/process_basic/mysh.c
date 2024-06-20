// mysh.c

#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#define DELIMS " \t\n"

typedef struct cmd_s {
    glob_t result;
} cmd_t;

static void prompt(void)
{
    printf("mysh - 0.1 >>> ");
}

static void parse(char *line, cmd_t *cmd)
{
    // ls -l -a -h /home/xyz/
    int i = 0;
    char *tok = NULL;

    while (1) {
        tok = strsep(&line, DELIMS);
        if (tok == NULL) {
            break;
        }
        if (tok[0] == '\0') {
            continue;
        }

        if (glob(tok, GLOB_NOCHECK | GLOB_APPEND * i, NULL, &cmd->result) != 0) {
            break;
        }
        i = 1;
    }
}

int main()
{
    int ret;

    while (1) {
        prompt();

        char *buffer = NULL;
        size_t buffer_size = 0;

        ret = getline(&buffer, &buffer_size, stdin);
        if (ret < 0) {
            break;
        }

        cmd_t cmd;
        parse(buffer, &cmd);

        if (0) {
            // ...
        } else {
            fflush(NULL);
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork()");
                exit(1);
            }

            if (pid == 0) {
                execvp(cmd.result.gl_pathv[0], cmd.result.gl_pathv);
                perror("execvp()");
                exit(1);
            } else {
                wait(NULL);
            }
        }
    }

    exit(0);
}