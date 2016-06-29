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
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include "machine.h"
#include "mem.h"
#include "trace.h"
#include "instruction.h"
#include "misc.h"

#define __DEBUG__

char AsciiHex[256][2] = {
  { '0', '0' },
  { '0', '1' },
  { '0', '2' },
  { '0', '3' },
  { '0', '4' },
  { '0', '5' },
  { '0', '6' },
  { '0', '7' },
  { '0', '8' },
  { '0', '9' },
  { '0', 'a' },
  { '0', 'b' },
  { '0', 'c' },
  { '0', 'd' },
  { '0', 'e' },
  { '0', 'f' },
  { '1', '0' },
  { '1', '1' },
  { '1', '2' },
  { '1', '3' },
  { '1', '4' },
  { '1', '5' },
  { '1', '6' },
  { '1', '7' },
  { '1', '8' },
  { '1', '9' },
  { '1', 'a' },
  { '1', 'b' },
  { '1', 'c' },
  { '1', 'd' },
  { '1', 'e' },
  { '1', 'f' },
  { '2', '0' },
  { '2', '1' },
  { '2', '2' },
  { '2', '3' },
  { '2', '4' },
  { '2', '5' },
  { '2', '6' },
  { '2', '7' },
  { '2', '8' },
  { '2', '9' },
  { '2', 'a' },
  { '2', 'b' },
  { '2', 'c' },
  { '2', 'd' },
  { '2', 'e' },
  { '2', 'f' },
  { '3', '0' },
  { '3', '1' },
  { '3', '2' },
  { '3', '3' },
  { '3', '4' },
  { '3', '5' },
  { '3', '6' },
  { '3', '7' },
  { '3', '8' },
  { '3', '9' },
  { '3', 'a' },
  { '3', 'b' },
  { '3', 'c' },
  { '3', 'd' },
  { '3', 'e' },
  { '3', 'f' },
  { '4', '0' },
  { '4', '1' },
  { '4', '2' },
  { '4', '3' },
  { '4', '4' },
  { '4', '5' },
  { '4', '6' },
  { '4', '7' },
  { '4', '8' },
  { '4', '9' },
  { '4', 'a' },
  { '4', 'b' },
  { '4', 'c' },
  { '4', 'd' },
  { '4', 'e' },
  { '4', 'f' },
  { '5', '0' },
  { '5', '1' },
  { '5', '2' },
  { '5', '3' },
  { '5', '4' },
  { '5', '5' },
  { '5', '6' },
  { '5', '7' },
  { '5', '8' },
  { '5', '9' },
  { '5', 'a' },
  { '5', 'b' },
  { '5', 'c' },
  { '5', 'd' },
  { '5', 'e' },
  { '5', 'f' },
  { '6', '0' },
  { '6', '1' },
  { '6', '2' },
  { '6', '3' },
  { '6', '4' },
  { '6', '5' },
  { '6', '6' },
  { '6', '7' },
  { '6', '8' },
  { '6', '9' },
  { '6', 'a' },
  { '6', 'b' },
  { '6', 'c' },
  { '6', 'd' },
  { '6', 'e' },
  { '6', 'f' },
  { '7', '0' },
  { '7', '1' },
  { '7', '2' },
  { '7', '3' },
  { '7', '4' },
  { '7', '5' },
  { '7', '6' },
  { '7', '7' },
  { '7', '8' },
  { '7', '9' },
  { '7', 'a' },
  { '7', 'b' },
  { '7', 'c' },
  { '7', 'd' },
  { '7', 'e' },
  { '7', 'f' },
  { '8', '0' },
  { '8', '1' },
  { '8', '2' },
  { '8', '3' },
  { '8', '4' },
  { '8', '5' },
  { '8', '6' },
  { '8', '7' },
  { '8', '8' },
  { '8', '9' },
  { '8', 'a' },
  { '8', 'b' },
  { '8', 'c' },
  { '8', 'd' },
  { '8', 'e' },
  { '8', 'f' },
  { '9', '0' },
  { '9', '1' },
  { '9', '2' },
  { '9', '3' },
  { '9', '4' },
  { '9', '5' },
  { '9', '6' },
  { '9', '7' },
  { '9', '8' },
  { '9', '9' },
  { '9', 'a' },
  { '9', 'b' },
  { '9', 'c' },
  { '9', 'd' },
  { '9', 'e' },
  { '9', 'f' },
  { 'a', '0' },
  { 'a', '1' },
  { 'a', '2' },
  { 'a', '3' },
  { 'a', '4' },
  { 'a', '5' },
  { 'a', '6' },
  { 'a', '7' },
  { 'a', '8' },
  { 'a', '9' },
  { 'a', 'a' },
  { 'a', 'b' },
  { 'a', 'c' },
  { 'a', 'd' },
  { 'a', 'e' },
  { 'a', 'f' },
  { 'b', '0' },
  { 'b', '1' },
  { 'b', '2' },
  { 'b', '3' },
  { 'b', '4' },
  { 'b', '5' },
  { 'b', '6' },
  { 'b', '7' },
  { 'b', '8' },
  { 'b', '9' },
  { 'b', 'a' },
  { 'b', 'b' },
  { 'b', 'c' },
  { 'b', 'd' },
  { 'b', 'e' },
  { 'b', 'f' },
  { 'c', '0' },
  { 'c', '1' },
  { 'c', '2' },
  { 'c', '3' },
  { 'c', '4' },
  { 'c', '5' },
  { 'c', '6' },
  { 'c', '7' },
  { 'c', '8' },
  { 'c', '9' },
  { 'c', 'a' },
  { 'c', 'b' },
  { 'c', 'c' },
  { 'c', 'd' },
  { 'c', 'e' },
  { 'c', 'f' },
  { 'd', '0' },
  { 'd', '1' },
  { 'd', '2' },
  { 'd', '3' },
  { 'd', '4' },
  { 'd', '5' },
  { 'd', '6' },
  { 'd', '7' },
  { 'd', '8' },
  { 'd', '9' },
  { 'd', 'a' },
  { 'd', 'b' },
  { 'd', 'c' },
  { 'd', 'd' },
  { 'd', 'e' },
  { 'd', 'f' },
  { 'e', '0' },
  { 'e', '1' },
  { 'e', '2' },
  { 'e', '3' },
  { 'e', '4' },
  { 'e', '5' },
  { 'e', '6' },
  { 'e', '7' },
  { 'e', '8' },
  { 'e', '9' },
  { 'e', 'a' },
  { 'e', 'b' },
  { 'e', 'c' },
  { 'e', 'd' },
  { 'e', 'e' },
  { 'e', 'f' },
  { 'f', '0' },
  { 'f', '1' },
  { 'f', '2' },
  { 'f', '3' },
  { 'f', '4' },
  { 'f', '5' },
  { 'f', '6' },
  { 'f', '7' },
  { 'f', '8' },
  { 'f', '9' },
  { 'f', 'a' },
  { 'f', 'b' },
  { 'f', 'c' },
  { 'f', 'd' },
  { 'f', 'e' },
  { 'f', 'f' }
};

char *ActionName[] = {
  "pc",
  "ac",
  "y",
  "x",
  "sp",
  "sr"
};

enum TraceStates TraceState = TRACE_OFF;
enum TraceTypes TraceType;

void trace_on (void) { TraceState = TRACE_ON;  }

void trace_sv(void)  { TraceType = TRACE_STATE_VEC;  }
void trace_act(void) { TraceType = TRACE_ACTION; }


int
trace_open(TraceHandle *th, char *file, int wflag)
{
  TraceHandle h;

  if (TraceState == TRACE_OFF) return 0;
  h = (struct Trace *)malloc(sizeof(struct Trace));
  bzero(h, sizeof(struct Trace));

  if (wflag) { 
    if (file) h->fd = open(file, O_CREAT|O_TRUNC|O_WRONLY, 0666);
    else      h->fd = STDOUT_FILENO;
  }
  else {
    if (file) h->fd = open(file, O_RDONLY);
    else      h->fd = STDIN_FILENO;
  }

  if (h->fd < 0) {
    free(h);
    fprintf(stderr, "ERROR: %s: %p, %s, %d\n",
	    __func__, h, file, wflag);
    perror(file);
    return -1;
  }
  *th = h;
  return h->fd;
}

static inline int
trace_read_from(TraceHandle th, int start)
{
  int rc;

  if (th==NULL) return 0;

  th->n = start;
  
  while (th->n < (TRACE_BUFFER_SIZE - start)) {
    rc = read(th->fd, &(th->buffer[th->n]), TRACE_BUFFER_SIZE - th->n);
    if (rc == 0) break;
    if (rc < 0 && errno != EINTR) return rc;
    th->n += rc;
  }
  return (th->n - start);
}

static inline int
trace_read(TraceHandle th)
{
  return trace_read_from(th, 0);
}

int 
trace_close(TraceHandle th) 
{
  int rc = 1;

  if (th) {
    if (th->n) trace_flush(th);
    if (th->fd != STDOUT_FILENO && 
	th->fd != STDIN_FILENO && 
	th->fd != STDERR_FILENO) {
      rc = close(th->fd);
    }
    free(th);
  }
  return rc;
}

#define TRACE_SV_SIZE (sizeof(union vstate))

void
trace_dump_sv(char *file)
{
  int v, i, svBytes;
  TraceHandle in, out;

  if (trace_open(&in, file, 0) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, %d\n",
	    __func__, &in, file, 0);
    return;
  }

  if (trace_open(&out, NULL, 1) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, NULL, %d\n",
	    __func__, &out, 1);
    return;
  }

  svBytes=0;
  while (trace_read(in) > 0) {
    for (i=0; i<in->n; i++) {
      v = in->buffer[i];
      if (trace_write(out, AsciiHex[v], 2) < 0) {
	fprintf(stderr, "ERROR: %s: trace_write: %p, %d\n",
		__func__, out, 2);
	return;
      }
      svBytes++;
      if (svBytes == TRACE_SV_SIZE) { 
	svBytes=0;
	if (trace_write(out, "\n", 1) < 0) {
	  fprintf(stderr, "ERROR: %s: trace_write: %p, %d\n",
		  __func__, out, 1);
	  return;
	}
      }
    }
  }
  trace_close(out);
  trace_close(in);
}


#define offsetof(type, member) __builtin_offsetof (type, member)

static inline long sv_mem_offset(address addr) {
  return offsetof(typeof(m.vs.ms), memory) + addr;	
}

#define DEFINE_REG_OFFSETS(REG)			\
  static inline long sv_ ## REG ## _offset() {	\
    return offsetof(typeof(m.vs.ms), reg) + offsetof(typeof(m.vs.ms.reg.data), REG); \
  }
DEFINE_REG_OFFSETS(pc);
DEFINE_REG_OFFSETS(ac);
DEFINE_REG_OFFSETS(x);
DEFINE_REG_OFFSETS(y);
DEFINE_REG_OFFSETS(sp);
DEFINE_REG_OFFSETS(sr);

void
trace_dump_action(FILE *f, action_t *a)
{
  switch (a->hdr.id) {
  case T_REG_RD:
    fprintf(stderr, "REG RD: ");
    fprintf(stderr, "%s ", ActionName[a->addr]);
    break;
  case T_REG_WR:
    fprintf(stderr, "REG WR: ");
    fprintf(stderr, "%s ", ActionName[a->addr]);
    break;
  case T_MEM_RD:
    fprintf(stderr, "MEM RD: ");
    fprintf(stderr, "%"PRIx16" ", a->addr);
    break;
  case T_MEM_WR:
    fprintf(stderr, "MEM WR: ");
    fprintf(stderr, "addr: %"PRIx16" ", a->addr);
    break;
  default:
    fprintf(stderr, "**** UNKNOWN ****: ");
  }
    assert(a->hdr.len == 1 || a->hdr.len == 2);
    for (int v=a->hdr.len-1; v>=0; v--)
      fprintf(stderr, "%02"PRIx8, a->val[v]);
    fprintf(stderr, "\n");
}

void
trace_dump_actions(FILE *f, action_t *a, int num)
{
  for (int i=0; i<num; i++) {
    trace_dump_action(f, &(a[i]));
  }
}
 
int
trace_write_action(TraceHandle th)
{
  static uint64_t TotalActions = 0;
  if (TraceState == TRACE_OFF) return 1;

#ifdef __DEBUG__
  TotalActions += ACTCNT;
  fprintf(stderr, "%llu ir=0x%02x, ACTCNT=%d (", 
	  TotalActions,  m.ireg.ir, ACTCNT);
  dump_inst(&InstTable[OPCodeTable[m.ireg.ir].ins]);
  fprintf(stderr, " ");
  dump_am(&(AMTable[OPCodeTable[m.ireg.ir].am]));
  fprintf(stderr, ")\n");
  trace_dump_actions(stderr, m.ireg.actionRec, ACTCNT);
#endif
  
  return trace_write(th, m.ireg.actionRec, ACTCNT * ACTIONSIZE);
}

void
trace_extract_sv(char *ifile, char *ofile,
			  uintptr_t startsv, uintptr_t endsv)
{
  int i;
  union vstate *sv;
  TraceHandle in, out;
  uintptr_t svnum=0;

  if (!ofile) {
    fprintf(stderr, "ERROR: %s: %s, %s, %"PRIuPTR", %"PRIuPTR"\n",
	    __func__, ifile, ofile, startsv, endsv);
    return;
  }
  if (trace_open(&in, ifile, 0) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, 0\n",
	    __func__, &in, ifile);
    return;
  }
  if (trace_open(&out, ofile, 1) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, 1\n",
	    __func__, &out, ofile);
    return;
  }
  while (trace_read(in) > 0) {
    for (i=0; i<in->n; i+=TRACE_SV_SIZE, svnum++) {
      if (endsv && svnum >= endsv) break;
      if (svnum >= startsv) {
	sv = (union vstate *)&(in->buffer[i]);
        trace_write(out, sv->raw, TRACE_SV_SIZE);
      }
    }
  }
  trace_close(in);
  trace_close(out);

  fprintf(stderr, "%s: filtered state vectors %"PRIuPTR" to %"PRIuPTR"\n",
	  __func__, startsv, svnum);
}

int
sv_filter_mask(union vstate *sv, unsigned char mask,
	       int offset, int numbytes)
{
  int i;
  unsigned char val;

  for (i=0; i<numbytes; i++){
    val = sv->raw[offset+i];
    if (val & mask) return 1;
  }
  return 0;
}

void
trace_filter_mask(char *ifile, char *ofile, char *svfile,
		  uintptr_t startsv, uintptr_t endsv,
		  int offset, unsigned char mask, uintptr_t numbytes)
{
  int i,j,k,b;
  union vstate *sv;
  TraceHandle initsv, in, out=0;
  unsigned char val[numbytes], diffval[numbytes];
  uintptr_t fnum=0, svnum=0;

  if (trace_open(&initsv, svfile, 0) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, 0\n",
	    __func__, &initsv, svfile);
    return;
  }
  if (trace_open(&in, ifile, 0) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, 0\n",
	    __func__, &in, ifile);
    return;
  }
  if (ofile && (trace_open(&out, ofile, 1) < 0)) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, 1\n",
	    __func__, &out, ofile);
    return;
  }
  while ( (b=trace_read(initsv)) > 0) {
    if (b < sizeof(struct mstate)) {
      fprintf(stderr, "ERROR: %s: trace_read: read %u out of %lu bytes\n",
	      __func__, b, sizeof(struct mstate));
      return;
    }
  }
  sv = (union vstate *)&(initsv->buffer); 
  memcpy(val, &sv->raw[offset], numbytes);
  while (trace_read(in) > 0) {
    for (i=0; i<in->n; i+=TRACE_SV_SIZE, svnum++) {
      if (endsv && svnum >= endsv) break;
      sv = (union vstate *)&(in->buffer[i]); 
      memcpy(diffval, &sv->raw[offset], numbytes);
      for (k=0; k<numbytes; k++)
	val[k] ^= diffval[k];
      if (svnum >= startsv) {
	if (sv_filter_mask(sv, mask, offset, numbytes)) {
	  fnum++;
	  if (ofile) trace_write(out, sv->raw, TRACE_SV_SIZE);
	  printf("%"PRIuPTR" ", svnum);
	  for (j=numbytes-1; j>=0; j--) printf("%02x", val[j]);
	  printf(" ");
	  for (j=numbytes-1; j>=0; j--) printf("%02x", diffval[j]);
	  printf("\n");
	}
      }
    }
  }
  trace_close(initsv);
  trace_close(in);
  if (ofile) trace_close(out);

  fprintf(stderr, "%s: filtered %"PRIuPTR
	  " from state vector %"PRIuPTR" to %"PRIuPTR"\n",
	  __func__, fnum, startsv, svnum);
}

void
trace_mstate_sv(char *ifile, char *ofile)
{
  int i;
  uint8_t *sv=0;
  TraceHandle in, out;

  if (trace_open(&in, ifile, 0) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, %d\n",
	    __func__, &in, ifile, 0);
    return;
  }

  if (trace_open(&out, ofile, 1) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, %d\n",
	    __func__, &out, ofile, 1);
    return;
  }

  // get the last machine state vector
  i=0;
  while (trace_read(in) > 0) {
    i += in->n;
    if (in->n % i == 0) {
      trace_memcpy(sv, in->buffer + in->n - TRACE_SV_SIZE,
		   TRACE_SV_SIZE);
    }
  }
  trace_write(out, sv, TRACE_SV_SIZE);

  trace_close(out);
  trace_close(in);
}

int
apply_action(uint8_t *sv, uint8_t *buf, size_t n)
{
  int i, offset;
  action_t *action;
  
  for (i=0; i<(n/ACTIONSIZE); i++)
  {
    action = (action_t *)buf + i;
    if (action->hdr.id == T_REG_WR)
    {
      switch (action->addr)
      {
      case pc_ACTION:
	offset = sv_pc_offset();
	break;
      case ac_ACTION:
	offset = sv_ac_offset();
	break;
      case y_ACTION:
	offset = sv_y_offset();
	break;
      case x_ACTION:
	offset = sv_x_offset();
	break;
      case sp_ACTION:
	offset = sv_sp_offset();
	break;
      case sr_ACTION:
	offset = sv_sr_offset();
	break;
      default:
	fprintf(stderr, "ERROR: %s: no such register\n",
		__func__);
	return -1;
      }
      trace_memcpy(sv + offset, &(action->val), action->hdr.len);
    }
    else if (action->hdr.id == T_MEM_WR)
    {
      offset = sv_mem_offset(action->addr);
      trace_memcpy(sv + offset, &(action->val), action->hdr.len);
    }
  }
  return (n % ACTIONSIZE);
}

void
trace_mstate_act(char *ifile, char *ofile)
{
  int offset, rc, i;
  uint8_t *sv, *copyptr;
  TraceHandle in, out;

  if (trace_open(&in, ifile, 0) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, %d\n",
	    __func__, &in, ifile, 0);
    return;
  }

  if (trace_open(&out, ofile, 1) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, %d\n",
	    __func__, &out, ofile, 1);
    return;
  }

  if (trace_read(in) < TRACE_SV_SIZE) {
    fprintf(stderr, "ERROR: %s: trace_read less than TRACE_SV_SIZE=%lu bytes\n",
	    __func__, TRACE_SV_SIZE);
    return;
  }
  sv = (uint8_t *)malloc(TRACE_SV_SIZE);
  trace_memcpy(sv, in->buffer, TRACE_SV_SIZE);

  offset = TRACE_SV_SIZE;
  do {
    rc = apply_action(sv, in->buffer + offset, in->n - offset);
    copyptr = in->buffer + TRACE_BUFFER_SIZE - rc;
    for (i=0; i<rc; i++) *(in->buffer + i) = *(copyptr + i);
    offset=0;
  } while (trace_read_from(in, rc));

  if (rc) {
    fprintf(stderr, "ERROR: %s: rc=%d\n",
	    __func__, rc);
    return;
  }

  trace_write(out, sv, TRACE_SV_SIZE);
  
  trace_close(in);
  trace_close(out);
}
