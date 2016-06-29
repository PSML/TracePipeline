#include <conio.h>

int main()
{
    unsigned int i = 3;

    for (i; i > 0; i--) {
        cprintf("i=%u\n", i);
    }
/*
    if (i > 1) {
        cprintf("i=%d\n", i);
    }
*/
    return i;
}
