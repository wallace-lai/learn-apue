#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf save;

static void d(void)
{
    printf("%s() : BEGIN ...\n", __FUNCTION__);

    printf("%s() : JUMP NOW ...\n", __FUNCTION__);
    longjmp(save, 0);

    printf("%s() : END ...\n", __FUNCTION__);
}

static void c(void)
{
    printf("%s() : BEGIN ...\n", __FUNCTION__);
    printf("%s() : CALL d() ...\n", __FUNCTION__);
    d();
    printf("%s() : d() RETURNED ...\n", __FUNCTION__);
    printf("%s() : END ...\n", __FUNCTION__);
}

static void b(void)
{
    printf("%s() : BEGIN ...\n", __FUNCTION__);
    printf("%s() : CALL c() ...\n", __FUNCTION__);
    c();
    printf("%s() : c() RETURNED ...\n", __FUNCTION__);
    printf("%s() : END ...\n", __FUNCTION__);
}

static void a(void)
{
    int ret;
    printf("%s() : BEGIN ...\n", __FUNCTION__);

    ret = setjmp(save);
    if (ret == 0) {
        printf("%s() : CALL b() ...\n", __FUNCTION__);
        b();
        printf("%s() : b() RETURNED ...\n", __FUNCTION__);
    } else {
        printf("%s() : JUMPED BACK HERE WITH CODE %d\n", __FUNCTION__, ret);
    }

    printf("%s() : END ...\n", __FUNCTION__);
}

int main()
{
    printf("%s() : BEGIN ...\n", __FUNCTION__);
    printf("%s() : CALL a() ...\n", __FUNCTION__);
    a();
    printf("%s() : a() RETURNED ...\n", __FUNCTION__);
    printf("%s() : END ...\n", __FUNCTION__);
}
