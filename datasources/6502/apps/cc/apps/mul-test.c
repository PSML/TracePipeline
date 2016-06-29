/* mul-test.c -- Test the multiplication operator. */
#include <inttypes.h>
#include <conio.h>
#include "cprintf.h"

int main(void)
{
  unsigned count;
  uint32_t errors = 0;
    /* Actual test variables */
  register unsigned lhs = 0;
  register unsigned rhs = 0;
  register unsigned res;

  /* Clear the screen, and output an informational message. */
  cprintf ("This program does an exhaustive test of\r\n"
	   "the multiplication routine. It runs for\r\n"
	   "several days; so, please wait very\r\n"
	   "patiently (or, speed up your emulator).\r\n"
	   "\n"
	   "Progress: \n");
  cprintf("lhs=%u\n", lhs); 
  count=0; 
  do {
    /* Do one row of tests */
    res = 0;
    count++;
    if (count==10) { cprintf("%u\n", lhs); count=0; }
    do {
      if (lhs * rhs != res) {
	cprintf ("Error on %u * %u: %u != %u\r\n", lhs, rhs, lhs * rhs, res);
	errors++;
      }
      res += lhs;
    } while (++rhs != 0);
  } while (++lhs != 0);
  
  if (errors==0) cprintf("\nSUCCESS\n");
  return 0;
}


