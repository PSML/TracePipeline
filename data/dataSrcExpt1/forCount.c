#ifdef DEBUG
#include "../cprintf.h"
#endif

#ifndef MAX
#define MAX 10
#endif

#ifndef START
#define START 0
#endif 

int i;
int
main() {
  for(i=START;i<MAX; i++)
    {
#ifdef DEBUG
      cprintf("%d\n",i);
#endif
    }
  return 0;
}
