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
#include "mem.h"
#include "instruction.h"
#include "console.h"
#include "misc.h"

extern team_t team;

struct machine m;

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

static void print_usage(char *argv0)
{
  fprintf(stderr, "USAGE: %s <input trace file> <initial sv file>\n"
	  "  -o [file]            output trace file\n"
	  "  -s [num]             start at state vector num\n"
	  "                       if not specified, start at first state vector\n"
	  "  -e [num]             end at state vector num (not including num)\n"
	  "                       if not specified, end at last state vector\n"
	  "  -t <extract|mask>    filter type\n"
          "  -m <conio|addr|reg>  mask type\n"
	  "  -a [addr]            if masking address, mask at addr\n"
	  "  -b [numbytes]        if masking address, mask numbytes\n"
	  "  -r <pc|ac|x|y|sp|sr> if masking register, specify reg\n",
          argv0);
}

int main(int argc, char **argv)
{
  int opt;
  int extract, mask, coniomaskfilter, addrmaskfilter, regmaskfilter;
  uintptr_t startsv, endsv, masknumbytes;
  char *ifile, *ofile, *svfile, *maskaddr, *maskreg;

  extract         = 0;
  mask            = 0;
  coniomaskfilter = 0;
  addrmaskfilter  = 0;
  regmaskfilter   = 0;
  startsv         = 0;
  endsv           = 0;
  maskaddr        = 0;
  masknumbytes    = 0;
  maskreg         = 0;
  ifile           = TRACE_DEFAULT_IN;
  ofile           = TRACE_DEFAULT_OUT;
  svfile          = 0;

  while ((opt = getopt(argc, argv, "o:s:e:t:m:a:b:r:")) != -1) {
    switch (opt) {
    case 'o':
      ofile = optarg; break;
    case 's':
      startsv = atoi(optarg); break;
    case 'e':
      endsv = atoi(optarg); break;
    case 't':
      if (strncmp(optarg, "extract", 7) == 0)
	extract++;
      else if (strncmp(optarg, "mask", 4) == 0)
	mask++;
      break;
    case 'm':
      if (strncmp(optarg, "conio", 5) == 0)
	coniomaskfilter++;
      else if (strncmp(optarg, "addr", 4) == 0)
	addrmaskfilter++;
      else if (strncmp(optarg, "reg", 3) == 0)
	regmaskfilter++;
      break;
    case 'a':
      maskaddr = optarg; break;
    case 'b':
      masknumbytes = atoi(optarg); break;
    case 'r':
      maskreg = optarg; break;
    case '?':
      print_usage(argv[0]);
      return -1;
    default:
      abort();
    }
  }
  if (optind + 1 < argc) {
    ifile = argv[optind];
    svfile = argv[optind+1];
  } else {
    print_usage(argv[0]);
    return -1;
  }

  if (extract) {
    if (!ofile) {
      fprintf(stderr, "ERROR: Invalid output trace file.\n");
      return -1;
    }
    if (!(startsv && endsv)) {
      fprintf(stderr, "ERROR: Invalid start and end state vector num.\n");
      return -1;
    }
    trace_extract_sv(ifile, ofile, startsv, endsv);
    return 1;
  }

  if (mask) {
    int offset = -1;

    if (coniomaskfilter) {
      offset = sv_mem_offset(CONSOLE_IOADDR);
      masknumbytes = 1;
    }
    else if (addrmaskfilter) {
      if (!maskaddr || !masknumbytes) {
	fprintf(stderr, "ERROR: Invalid addr or numbytes.\n");
	return -1;
      }
      offset = sv_mem_offset(strtol(maskaddr, NULL, 0));
    }
    else if (regmaskfilter) {
      if (!maskreg) {
	fprintf(stderr, "ERROR: Invalid reg.\n");
	return -1;
      }
      masknumbytes = 1;
      if (strncmp(maskreg, "pc", 2) == 0) {
	offset = sv_pc_offset();
	masknumbytes = 2;
      }
      else if (strncmp(maskreg, "ac", 2) == 0)
	offset = sv_ac_offset();
      else if (strncmp(maskreg, "x", 1) == 0)
	offset = sv_x_offset();
      else if (strncmp(maskreg, "y", 1) == 0)
	offset = sv_y_offset();
      else if (strncmp(maskreg, "sp", 2) == 0)
	offset = sv_sp_offset();
      else if (strncmp(maskreg, "sr", 2) == 0)
	offset = sv_sr_offset();

      if (offset == -1) {
	fprintf(stderr, "ERROR: Invalid reg.\n");
	return -1;
      }
    }
    else {
      fprintf(stderr, "ERROR: Invalid mask type.\n");
	return -1;
    }
    trace_filter_mask(ifile, ofile, svfile, startsv, endsv, offset, 0xFF, masknumbytes);
    return 1;
  }

  print_usage(argv[0]);
  return 0;
}
