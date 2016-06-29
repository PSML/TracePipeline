#ifndef __TRACE_H__
#define __TRACE_H__
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
#include <unistd.h>
#include <errno.h>

//#define TRACE_IR 1
#define TRACE_BUFFER_SIZE (65544 * 24)

#ifdef __DEBUG__
#  define DLOG(...) fprintf(stderr, __VA_ARGS__)
#  define DFUNC(f)  f
#else
#  define DLOG(...)
#  define DFUNC(f)
#endif

struct Trace {
  int fd;
  uint8_t buffer[TRACE_BUFFER_SIZE];
  size_t n;
};

typedef struct Trace *TraceHandle;
TraceHandle TraceTh;

enum TraceStates {TRACE_OFF, TRACE_ON};
extern enum TraceStates TraceState;

enum TraceTypes {TRACE_STATE_VEC, TRACE_ACTION};
extern enum TraceTypes TraceType;

int trace_open(TraceHandle *th, char *file, int wflag);
int trace_close(TraceHandle th);
void trace_on(void);
void trace_sv(void);
void trace_act(void);
void trace_dump_sv(char *ifile);
void trace_paint_sv(char *file, uint8_t zoom, char *bmp_prefix);
int trace_write_action(TraceHandle th);
void trace_mstate_sv(char *file, char *ofile);
void trace_mstate_act(char *file, char *ofile);

void trace_extract_sv(char *ifile, char *ofile,
		      uintptr_t startsv, uintptr_t endsv);
void trace_filter_mask(char *ifile, char *ofile, char *svfile,
		       uintptr_t startsv, uintptr_t endsv,
		       int offset, unsigned char mask, uintptr_t numbytes);


static inline int
trace_flush(TraceHandle th)
{
  int n=0;
  int rc = 0;

  if (th==NULL) return rc;

  while (th->n) {
    rc = write(th->fd, &(th->buffer[n]), th->n);
    if (rc == 0 || (rc < 0 && errno != EINTR)) {
      perror("trace_flush");
      break;
    }
    if (rc > 0)  {
      th->n -= rc;
      n += rc;
    }
  }
  return rc;
}

static inline void
trace_memcpy(void *dst, void *src, size_t len)
{
  // FIXME: optimize as desired using cache and vector friendly 
  //        support
  memcpy(dst, src, len);
}

static inline int 
trace_write(TraceHandle th, void *buf, size_t len)
{

  if (th == NULL) return 0;

  if (len > TRACE_BUFFER_SIZE) return -1;

  if (th->n + len >= TRACE_BUFFER_SIZE)  {
    int rc = trace_flush(th);
    if (rc < 0) return rc;
    if (th->n + len > TRACE_BUFFER_SIZE) return -1;
  }

  trace_memcpy(&(th->buffer[th->n]), buf, len);
  th->n += len;

  return len;
}

static inline int
trace_write_sv(TraceHandle th)
{
  return trace_write(th, m.vs.raw, sizeof(m.vs.raw));
}


inline static int
trace()
{
  if (TraceState == TRACE_ON) {
    switch(TraceType)
    {
    case TRACE_STATE_VEC:
      return trace_write_sv(TraceTh);
    case TRACE_ACTION:
      return trace_write_action(TraceTh);
    }
  }
  return 0;
}

#endif
