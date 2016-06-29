#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "cprintf.h"

#define printf cprintf
#define fprintf(stderr, ...) printf(__VA_ARGS__)

static int do_test(const char *s1, const char *s2, size_t n)
{
    printf("strnicmp(\"%s\", \"%s\", %d):  ", s1, s2, (int)n);
    return strncasecmp(s1, s2, n);
}

int main(void)
{
    int ret;
    int Failures = 0;

    ret = do_test("Wurzl", "wURZL", 5);
    if (ret) {
      Failures++;
        printf("fail (%d)\n", ret);
    }
    else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzl", "wURZL", 6);
    if (ret) {
      Failures++;
        printf("fail (%d)\n", ret);
    } else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzl", "wURZL", 10);
    if (ret) {
      Failures++;
        printf("fail (%d)\n", ret);
    } else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzla", "wURZLB", 10);
    if (ret >= 0) {
      Failures++;
        printf("fail (%d)\n", ret);
    } else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzla", "wURZLb", 5);
    if (ret) {
      Failures++;
        printf("fail (%d)\n", ret);
    } else
        printf("OK (%d)\n", ret);

    ret = do_test("BLI", "bla", 5);
    if (ret <= 0) {
      Failures++;
        printf("fail (%d)\n", ret);
    } else
        printf("OK (%d)\n", ret);

    ret = do_test("", "bla", 5);
    if (ret >= 0) {
      Failures++;
        printf("fail (%d)\n", ret);
    } else
        printf("OK (%d)\n", ret);

    ret = do_test("BLI", "", 5);
    if (ret <= 0) {
      Failures++;
        printf("fail (%d)\n", ret);
    } else
        printf("OK (%d)\n", ret);

    ret = do_test("", "", 5); 
    if (ret) {
      Failures++;
      printf("fail (%d)\n", ret);
    } else
      printf("OK (%d)\n", ret);

    if (Failures==0) printf("\nSUCCESS\n");

    return 0;
}
