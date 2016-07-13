#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include "cprintf.h"

uint32_t cnt=0;

void parity(void) {
  if (cnt & 0x1) cprintf("ODD\n");
  else cprintf("EVEN\n");
}

int
main() {
  uint8_t c, q=0;

  while (!q) {
    c = cgetc();
    switch (c) {
    case 'E':
      cnt++;
      break;
    case 'V':
      cprintf("%" PRIu32 "\n", cnt);
      break;
    case 'P':
      parity();
      break;
    case 'Z':
      cnt=0;
      break;
    case 'Q':
      q = 1;
    }
  }
  return 0;
}
