#include <stdio.h>

#define BEG 30000000
#define END 30000200

int main()
{
    int mark;

    for (int i = BEG; i <= END; i++) {
        mark = 1;
        for (int p = 2; p < i / 2; p++) {
            if (i % p == 0) {
                mark = 0;
                break;
            }
        }

        if (mark) {
            printf("%d is a primer.\n", i);
        }
    }

    return 0;
}