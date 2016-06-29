#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"

#define MAX 47

uint32_t fib(int32_t x)
{
  int16_t i;
  int32_t prev0,prev1,result;

  if ((int16_t)x == 0) return 0;

  prev0 = 0;
  prev1 = 1;
  result = 1;

  for (i=2; i<(int16_t)x; i++) {
    prev0 = prev1;
    prev1 = result;
    result = prev0 + prev1;
  }
  return result;
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
