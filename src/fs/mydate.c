#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define TIMESIZE   1024
#define FMTSTRSIZE 1024

/**
 * -y : year
 * -m : month
 * -d : day
 * -H : hour
 * -M : minute
 * -S : second
 */

static int bitopt[6];

int main(int argc, char *argv[])
{
    int c;
    time_t stamp;
    struct tm *tm;
    static char timestr[TIMESIZE];
    static char fmtstr[FMTSTRSIZE];

    memset(timestr, 0, TIMESIZE);
    memset(fmtstr, 0, FMTSTRSIZE);

    stamp = time(NULL);
    tm = localtime(&stamp);

    while ((c = getopt(argc, argv, "y:mdH:MS")) > 0) {
        switch (c) {
            case 'y':
                bitopt[0] = 1;
                break;
            case 'm':
                bitopt[1] = 1;
                break;
            case 'd':
                bitopt[2] = 1;
                break;
            case 'H':
                if (strcmp(optarg, "12") == 0) {
                    bitopt[3] = 12;
                } else {
                    bitopt[3] = 24;
                }
                break;
            case 'M':
                bitopt[4] = 1;
                break;
            case 'S':
                bitopt[5] = 1;
                break;
            default:
                break;
        }
    }

    if (bitopt[0]) {
        strcat(fmtstr, "Year : %Y\n");
    }
    if (bitopt[1]) {
        strcat(fmtstr, "Month : %m\n");
    }
    if (bitopt[2]) {
        strcat(fmtstr, "Day : %d\n");
    }
    if (bitopt[3] == 12) {
        strcat(fmtstr, "Hour : %I(%P)\n");
    } else {
        strcat(fmtstr, "Hour : %H\n");
    }
    if (bitopt[4]) {
        strcat(fmtstr, "Min : %M\n");
    }
    if (bitopt[5]) {
        strcat(fmtstr, "Sec : %S\n");
    }

    strftime(timestr, TIMESIZE, fmtstr, tm);
    puts(timestr);

    exit(0);
}