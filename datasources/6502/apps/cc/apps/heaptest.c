#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <_heap.h>
#include "cprintf.h"
#include <stdlib.h>

#define printf cprintf
#define getchar cgetc

static unsigned char* V[256];



static char* Alloc (void)
/* Allocate a random sized chunk of memory */
{
    /* Determine the size */
    unsigned char Size = (((unsigned char)rand()) & 0x7F) + 1;

    /* Allocate memory */
    unsigned char* P = malloc (Size);

    /* Set the string to a defined value. We use the size, since this will
     * also allow us to retrieve it later.
     */
    if (P) {
    	memset (P, Size, Size);
    } else {
    	printf ("Could not allocate %u bytes\n", Size);
    	exit (EXIT_FAILURE);
    }
    return P;
}



static void Free (unsigned char* P)
/* Check a memory block and free it */
{
    unsigned char I;

    /* Get the size of the block */
    unsigned char Size = P[0];

    /* Scan the block */
    for (I = 1; I < Size; ++I) {
    	if (P[I] != Size) {
    	    printf ("Scan failed - expected %02X, got %02X\n",
    	    	    Size, P[I]);
    	    exit (EXIT_FAILURE);
       	}
    }

    /* Free the block */
    free (P);
}



static void FillArray (void)
/* Fill the array with randomly allocated memory chunks */
{
    unsigned char I = 0;
    do {
    	V[I] = Alloc ();
    	++I;
    } while (I != 0);
}



static unsigned ShowInfo (void)
/* Show heap info */
{
    /* Count free blocks */
    unsigned Count = 0;
    register struct freeblock* P = _heapfirst;
    while (P) {
	++Count;
       	P = P->next;
    }
    printf ("%04X  %04X  %04X  %04X  %04X %u\n",
      	    _heaporg, _heapptr, _heapend, _heapfirst, _heaplast, Count);

    if (Count) {
	P = _heapfirst;
	while (P) {
	    printf ("%04X  %04X  %04X %04X(%u)\n",
		    (unsigned) P, P[2], P[1], P[0], P[0]);
	    P = P->next;
	}
    }
    return Count;
}



static unsigned Test1 (void)
{
    unsigned char I;
    FillArray ();
    for (I = 0; I < 0x80; ++I) {
	Free (V[0x7F-I]);
	Free (V[0x80+I]);
    }
    return ShowInfo ();
}



static unsigned Test2 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
       	Free (V[I]);
	++I;
    } while (I != 0);
    return ShowInfo ();
}



static unsigned Test3 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
     	--I;
     	Free (V[I]);
    } while (I != 0);
    return ShowInfo ();
}



static unsigned Test4 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
	Free (V[I]);
	I += 2;
    } while (I != 0);
    I = 1;
    do {
	Free (V[I]);
	I += 2;
    } while (I != 1);
    return ShowInfo ();
}



static unsigned Test5 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
	Free (V[I]);
	I += 2;
    } while (I != 0);
    do {
       	V[I] = Alloc ();
	I += 2;
    } while (I != 0);
    I = 1;
    do {
	Free (V[I]);
	I += 2;
    } while (I != 1);
    do {
       	V[I] = Alloc ();
	I += 2;
    } while (I != 1);
    I = 0;
    do {
      	Free (V[I]);
	++I;
    } while (I != 0);
    return ShowInfo ();
}



static unsigned Test6 (void)
{
    unsigned char I, J;
    FillArray ();
    I = J = 0;
    do {
	do {
	    Free (V[I]);
	    V[I] = Alloc ();
	    ++I;
	} while (I != 0);
	++J;
    } while (J < 5);
    do {
	Free (V[I]);
	++I;
    } while (I != 0);
    return ShowInfo ();
}



int main (void)
{
  unsigned count=0;
#if 0
    unsigned long T;
#endif

    /* Show info at start */
    count += ShowInfo ();

#if 0
    /* Remember the time */
    T = clock ();
#endif

    /* Do the tests */
    count += Test1 ();
    count += Test2 ();
    count += Test3 ();
    count += Test4 ();
    count += Test5 ();
    count += Test6 ();

    /* Calculate the time and print it */
#if 0
    T = clock () - T;
    printf ("Time needed: %lu ticks\n", T);
#endif
    if (count == 0) printf("\nSUCCESS\n");
    /* Done */
    return EXIT_SUCCESS;
}



