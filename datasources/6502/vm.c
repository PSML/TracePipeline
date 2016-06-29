#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "machine.h"
#include "mem.h"
#include "instruction.h"
#include "interrupt.h"
#include "console.h"
#include "trace.h"
#include "loop.h"
#include "misc.h"

// FIXME: JA VERY QUICK HACK TO SEE IF WE CAN TRIVIALLY HOOKUP 6502 to VM 
//        This can be dramatically optmized by avoiding mem copies

static struct machine m = {.vs = NULL};

int 
initial(uint8_t **row, char *const file, char *tracefile, int flag)
{
  if (misc_initialize(&m, file, tracefile, 0, 0, 0)<0) return -1;
  *row = m.vs->raw;
  return sizeof(m.vs->raw);
}

int 
evolution(void *y,  // output vector
	  void const *x,  // input vector
	  void *g,   // I think represents a count of which elements have been 
	             // modified.
	  int bytes, int flags)
{
  int rc;

  m.vs = (union vstate *)x;
  rc = loop(1, &m);
  y = m.vs->raw;
  if (rc<0) return rc;
  return m.ireg.ir;
}

int 
gdb(int sock, uint8_t * x, int bytes)
{
  assert(0);
  return -1;
}

uint32_t 
feature(uint8_t * x, int K, int i)
{
  assert(0);
  return -1;
}

int 
features(uint32_t * phi, uint8_t *x, int bytes)
{
  assert(0);
  return -1;
} 

/* Convert a feature vector into a state vector.  */
int 
statevector(uint8_t * x, uint32_t const *phi, int K)
{
  assert(0);
  return -1;
}
