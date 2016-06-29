#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"
#include "fibtbl.h"

#define MAX 47

uint32_t fib(int32_t x)
{
  return fibtbl[x];
}

int
main() {
  int32_t x;
  uint32_t ret;

  while(1) {
    if (cscanf("%"PRId32,&x)==0) break;
    if (x > MAX) cprintf("-1\n");
    else {
      ret = fib(x);
      cprintf("%"PRIu32"\n", ret);
    }
  }

  return 0;
}
