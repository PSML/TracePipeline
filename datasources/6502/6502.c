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
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "machine.h"
#include "trace.h"
#include "interrupt.h"
#include "mem.h"
#include "loop.h"
#include "instruction.h"
#include "console.h"
#include "misc.h"
#include "mstate.h"

extern team_t team;

struct machine m;

#define offsetof(type, member)  __builtin_offsetof (type, member)

static void pr_reginfo(void) 
{
  printf("%s:%ld,%ld\n", "pc",		
	 offsetof(typeof(m.vs.ms.reg.data), pc),	
	 sizeof(m.vs.ms.reg.data.pc));

  printf("%s:%ld,%ld\n", "ac",		
	 offsetof(typeof(m.vs.ms.reg.data), ac),	
	 sizeof(m.vs.ms.reg.data.ac));

  printf(" %s:%ld,%ld\n", "y",		
	 offsetof(typeof(m.vs.ms.reg.data), y),	
	 sizeof(m.vs.ms.reg.data.y));

  printf(" %s:%ld,%ld\n", "x",		
	 offsetof(typeof(m.vs.ms.reg.data), x),	
	 sizeof(m.vs.ms.reg.data.x));

  printf("%s:%ld,%ld\n", "sp",		
	 offsetof(typeof(m.vs.ms.reg.data), sp),	
	 sizeof(m.vs.ms.reg.data.sp));

  printf("%s:%ld,%ld\n", "sr",		
	 offsetof(typeof(m.vs.ms.reg.data), sr),	
	 sizeof(m.vs.ms.reg.data.sr));

  printf("%s:%ld,%ld\n", "memory",		
	 offsetof(typeof(m.vs.ms), memory),	
	 sizeof(m.vs.ms.memory));
}

#if 0
static inline long sv_mem_offset(address addr) 
{
  return offsetof(typeof(m.vs.ms), memory) + addr;	
}

#define DEFINE_REG_OFFSETS(REG)			\
  static inline long sv_ ## REG ## _offset() {	\
    return offsetof(typeof(m.vs.ms), reg) + offsetof(typeof(m.vs.ms.reg.data), REG);\
  }

DEFINE_REG_OFFSETS(pc);
DEFINE_REG_OFFSETS(ac);
DEFINE_REG_OFFSETS(x);
DEFINE_REG_OFFSETS(y);
DEFINE_REG_OFFSETS(sp);
DEFINE_REG_OFFSETS(sr);
#endif

static void print_usage(char *argv0)
{
  fprintf(stderr, "USAGE: %s [input memory image] [output memory image]\n"
          "  -t <sv|act> output binary file of the machine state\n"
          "             in the format of a State Vector or an Execution Record\n"
          "  -m <sv|act> output binary file containing the last state vector\n"
          "             of the specified input binary file\n"
          "  -o [file]  specify output binary file (default 6502.trc.bin)\n"
	  "  -p [file]  specify program output file (default stdout)\n"
	  "  -i [file]  specify input binary file\n"
          "  -l         load a state vector from the specified input binary file\n"
          "             and start execution\n"
          "  -c [num]   execute num instructions\n"
          "             if zero, will simulate until break\n"
          "  -d <sv|act> dump trace file 6502.bin as ascii hex byte values\n"
          "  -v         list visible state vector machine components:\n"
          "               name, offset, and length in bytes\n",
          argv0);
}

int
main(int argc, char **argv)
{
  int opt, n;
  int traceSV, traceACT, lastSV, lastACT, lflag, dumpSV, dumpER, vflag;
  char *inimg, *outimg, *count, *ifile, *ofile, *pfile;

  traceSV         = 0;
  traceACT        = 0;
  lastSV          = 0;
  lastACT         = 0;
  lflag           = 0;
  dumpSV          = 0;
  dumpER          = 0;
  vflag           = 0;
  inimg           = 0;
  outimg          = 0;
  count           = 0;
  ifile           = 0;
  ofile           = 0;
  pfile           = 0;
  n = 0;

  while ((opt = getopt(argc, argv, "i:o:p:t:m:lc:d:v")) != -1) {
    switch (opt) {
    case 'i':
      ifile = optarg; break;
    case 'o':
      ofile = optarg; break;
    case 'p':
      pfile = optarg; break;
    case 't':
      if (strncmp(optarg, "sv", 2) == 0)
	traceSV++;
      else if (strncmp(optarg, "act", 3) == 0)
	traceACT++;
      break;
    case 'm':
      if (strncmp(optarg, "sv", 2) == 0)
	lastSV++;
      else if (strncmp(optarg, "act", 3) == 0)
	lastACT++;
      break;
    case 'l':
      lflag  = 1; break;
    case 'c':
      count  = optarg; break;
    case 'd':
      if (strncmp(optarg, "sv", 2) == 0)
	dumpSV++;
      break;
    case 'v':
      vflag  = 1; break;
    case '?':
      print_usage(argv[0]);
      return -1;
    default:
      abort();
    }
  }

  if (lastSV || lastACT) {
    if (lastSV)      trace_mstate_sv(ifile, ofile);
    else             trace_mstate_act(ifile, ofile);
    if (!(dumpSV || dumpER || vflag)) return 1;
  }

  if (dumpSV) {
    fprintf(stderr, "Dumping trace file:\n");
    if (dumpSV) trace_dump_sv(ifile);
    if (!vflag) return 1;
  }

  if (vflag) {
    fprintf(stderr, "Listing state vector components:\n");
    pr_reginfo();
    return 1;
  }

  if (optind+1 < argc) {
    inimg  = argv[optind];
    outimg = argv[optind+1];
  } else {
    print_usage(argv[0]);
    return -1;
  }
  if (traceSV || traceACT) {
    trace_on();
    if (traceSV)  trace_sv();
    if (traceACT) trace_act();
  }
  if (count) n = atoi(count);

  if (TraceState == TRACE_ON &&
      TraceType == TRACE_ACTION) {
    memset(m.ireg.actionRec, 0,
	   MAX_DATA_ACCESSES * ACTIONSIZE);
    ACTCNT = 0;
  }
  if (misc_initialize(inimg, pfile, ofile,
		      1, // enable interrupt signals
		      lflag) < 0) {
    fprintf(stderr, "ERROR: initialization failure\n");
    return -1;
  }
#ifdef __DEBUG__
  memcpy(prev_sv, m.vs.raw, sizeof(m.vs.raw));
#endif

  loop(n);

  if (misc_finalize(outimg) < 0) {
    fprintf(stderr, "ERROR: finalization failure\n");
    return -1;
  }

  return 0;
}
