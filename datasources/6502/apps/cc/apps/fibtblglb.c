#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"
#include "fibtbl.h"

#define MAX ((sizeof(fibtbl)/sizeof(uint32_t))-1)

int global_counter = 0;

uint32_t fib(int32_t x)
{
  global_counter++;
  if (x > MAX) return -1;
  return fibtbl[x];
}

int
main() {
  int32_t x;
  uint32_t ret;

  while(1) {
    cscanf("%"PRId32,&x);
    if (x < 0) break;
    ret = fib(x);
    cprintf("%"PRIu32"\n", ret);
  }

  return 0;
}
