#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"

int
main() {
  int32_t i, x;

  for (i=0; i<3; i++) {
    cscanf("%"PRId32,&x);
    cprintf("%"PRId32, x + 10);
  }

  return 0;
}
