#ifdef DEBUG
#include "../cprintf.h"
#endif

#ifndef MAX
#define MAX 10
#endif

#ifndef START
#define START 0
#endif

int i = START;

void r(){
#ifdef DEBUG
  cprintf("%d\n",i);
#endif

  if(i == MAX)
    return;
  i++;
  r();
}

int main() {
  r();
  return 0;
}
