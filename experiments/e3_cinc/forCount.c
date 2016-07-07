#ifdef DEBUG
#include "../cprintf.h"
#endif

#ifndef INC
#define INC 1
#endif

#ifndef MAX
#define MAX 10*INC+Start
#endif

#ifndef START
#define START 0
#endif 

int i;
int
main() {
  for(i=START;i<MAX; i+=INC)
    {
#ifdef DEBUG
      cprintf("%d\n",i);
#endif
    }
  return 0;
}
