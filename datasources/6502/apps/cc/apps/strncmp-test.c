#include <stdlib.h>
#include <string.h>
#include "cprintf.h"

#define printf cprintf
#define fprintf(stderr, ...) printf(__VA_ARGS__)


static const char S1[] = {
    'h', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0', 'A'
};

static const char S2[] = {
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\0', 'B'
};




int main (void)
{
  int ecnt=0;
  int ncnt=0;
    char I;
    for (I = 0; I < 20; ++I) {
      	printf ("%02d: %d\n", I, strncmp (S1, S2, I));
	if (strncmp (S1, S2, I)==0) ecnt++; else ncnt++;
    }
    if (ecnt == 7 && ncnt == 13) printf("\nSUCCESS\n"); 
    else printf("ecnt=%d ncnt=%d\n", ecnt, ncnt);
    return 0;
}

