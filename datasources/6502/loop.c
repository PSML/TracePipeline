/******************************************************************************
* Copyright (C) 2011 by Jonathan Appavoo, Boston University
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*****************************************************************************/
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#include "machine.h"
#include "trace.h"
#include "mem.h"
#include "instruction.h"
#include "interrupt.h"
#include "misc.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "",
    /* First member's full name */
    "",
    /* First member's email address */
    "",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* CODE: BEGIN */

#ifdef IMP
int
loop(uint64_t count)
{
  uint64_t i = 0;
  clock_t start, end;
  double timer;

  // initial state vector
  trace();

  // start timer
  start = clock();

  i = inst_loop(count);

  // end timer
  end = clock();
  timer = (float)(end - start) / CLOCKS_PER_SEC;
  fprintf(stderr, "performance: %f sec inst %"PRIu64" ips %f MIPS\n",
	  timer, i, ((double)i/timer)/1000000);
  fprintf(stderr, "EXITING: count=%"PRIu64" i=%"PRIu64"\n", count, i);

  return i;
}
#else
int 
loop(int count)
{
  int rc = 1;
  unsigned int i = 0;

  if (count<0) return rc;

  while (1) {
    TRACE_LOOP(dump_cpu());
    rc = interrupts();

    put your code here

    i++;
    if (rc < 0 || (count && i == count)) break;
  }
  VPRINT("EXITING: count=%d i=%d\n",count, i);
  return rc;
}

#endif
