/* A small test for atoi. Assumes twos complement */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "cprintf.h"



static unsigned int Failures = 0;



static void CheckAtoi (const char* Str, int Val)
{
    int Res = atoi (Str);
    if (Res != Val) {
        cprintf ( "atoi error in \"%s\":\n"
                          "  result = %d, should be %d\n", Str, Res, Val);
        ++Failures;
    }
}



int main (void)
{
    CheckAtoi ("\t +0A", 0);
    CheckAtoi ("\t -0.123", 0);
    CheckAtoi ("  -32  ", -32);
    CheckAtoi (" +32  ", 32);
    CheckAtoi ("0377", 377);
    CheckAtoi (" 0377 ", 377);
    CheckAtoi (" +0377 ", 377);
    CheckAtoi (" -0377 ", -377);
    CheckAtoi ("0x7FFF", 0);
    CheckAtoi (" +0x7FFF", 0);
    CheckAtoi (" -0x7FFF", 0);
    cprintf ("Failures: %u\n", Failures);
    if (Failures==0) cprintf("SUCCESS\n");
    return (Failures != 0);
}
