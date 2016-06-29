#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include "fibtbl.h"

#define MAX ((sizeof(fibtbl)/sizeof(uint32_t))-1)

int
main()
{
  int32_t i;
  uint32_t v;

  for (i=0; i<=MAX; i++)    {
    v = fibtbl[i];
    printf("     .WORD $%04x, $%04x   ; fib %d -> %u\n",
	   0x0000FFFF & v, (0xFFFF0000 & v) >> 16, i, v);
  }
  printf("     ; %lu values occuping %lu bytes\n", MAX+1, sizeof(fibtbl)); 
  return 0;
}
     
