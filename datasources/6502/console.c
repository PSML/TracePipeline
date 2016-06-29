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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "machine.h"
#include "trace.h"
#include "mem.h"
#include "instruction.h"
#include "interrupt.h"
#include "misc.h"
#include "tty.h"
#include "console.h"

/* CODE: BEGIN */

struct console 
{
  address addr;
  int last_store_val;
  int num_store;
  int last_load_val;
  int num_load;
  int load_fd;
  int store_fd;
};

#ifndef IMP
static int 
console_store(void *ptr, byte v) __attribute__ ((unused));
static int 
console_load(void *ptr, byte *v) __attribute__ ((unused));
#endif

#ifdef JA_CONCTL_HACK
static void
console_ctl_load(void *ptr, byte *v)
{
  int n=0;
  struct console *c;
  
  assert(ptr);
  c = ptr;
  ioctl(c->load_fd, FIONREAD, &n);
  *v = (n>0) ? -1 : 0;
}

static void
console_ctl_store(void *ptr, byte v)
{
}
#endif

static void
console_store(void *ptr, byte v)
{
  int rc;
  struct console *c;

  assert(ptr);
  c = ptr;

  rc = write(c->store_fd, &v, 1);
  if (rc == 1) {
    c->last_store_val = v;
    c->num_store++;
  }
}

static void
console_load(void *ptr, byte *v)
{
  int rc;
  struct console *c;

  assert(ptr);
  c = ptr;
  rc = read(c->load_fd, &(c->last_load_val), 1);
  if (rc == 1) {
    c->num_load++;
    *v = c->last_load_val;
  } else {
    *v = 0;
  }
#if defined(__DEBUG__) || defined(__CONSOLE_LOAD_ACTION__)
  if (TraceState == TRACE_ON && TraceType == TRACE_ACTION) {
    ACTION(ACTCNT).hdr.id  = T_MEM_WR;
    ACTION(ACTCNT).hdr.len = sizeof(byte);
    ACTION(ACTCNT).addr = CONSOLE_IOADDR;
    memcpy(ACTION(ACTCNT).val, v, ACTION(ACTCNT).hdr.len);
    ACTCNT++;
  }
#endif
}

int
console_init(char *progfile)
{
  struct console *c;

  c = (struct console *)malloc(sizeof(struct console));
  if (c == NULL) return 0;
  
  bzero(c, sizeof(struct console));
  c->addr     = CONSOLE_IOADDR;
  c->load_fd  = STDIN_FILENO;
 
  if (progfile) c->store_fd = open(progfile, O_CREAT|O_TRUNC|O_WRONLY, 0666);
  else          c->store_fd = STDOUT_FILENO;

  tty_init(c->store_fd);

#ifdef IMP
  VPRINT("mapping console at 0x%04x\n", c->addr);
  (void)mem_map_device(CONSOLE_IOADDR, c, console_load, console_store);
#ifdef JA_CONCTL_HACK
  (void)mem_map_device(CONSOLE_CTLADDR, c, console_ctl_load, console_ctl_store);
#endif
  return 1;
#else
  return 1;
#endif
}

