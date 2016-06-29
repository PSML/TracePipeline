#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"

int main()
{
   uint32_t i;
   uint32_t j;
   
   for (i = 1; i < 32; i++) {
     //cprintf("i=%" PRId32 "\n", i);
     for (j = i; j > 1; ) {
       //cprintf("j=%" PRId32, j);
       if (j % 2 == 0) {
	 j = j / 2;
         //cprintf(" 1:j=%" PRId32 "\n", j);
       }
       else {
	 j = 3 * j + 1;
         //cprintf(" 2:j=%" PRId32 "\n", j);
       }
     }
   }

   cprintf("i=%" PRId32 ", j=%" PRId32 "\n", i, j);

   return i;
}
