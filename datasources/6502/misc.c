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
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>


#include "machine.h"
#include "trace.h"
#include "mem.h"
#include "mstate.h"
#include "instruction.h"
#include "console.h"
#include "misc.h"
#include "interrupt.h"

static int dump_cnt = 0;

int 
dump_sr(byte sr)
{
  fprintf(stderr, "sr: 0x%02x: N=%01d V=%01d B=%01d D=%01d I=%01d Z=%01d C=%01d\n", sr,
	  SR_N(sr), SR_V(sr), SR_B(sr), SR_D(sr), 
	  SR_I(sr), SR_Z(sr), SR_C(sr));
  return 1;
}

int 
dump_reg()
{
  int rc = dump_cnt;
  fprintf(stderr, "ac: 0x%02x y: 0x%02x x: 0x%02x pc: 0x%02x,0x%02x, sp: 0x%02x\n",
	  machine_ac_get(), machine_y_get(), machine_x_get(),
          ADDR_HIGH(machine_pc_get()), ADDR_LOW(machine_pc_get()), machine_sp_get());
  dump_sr(machine_sr_get());
  dump_cnt++;
  return rc;
}

int 
dump_cpu()
{
  int rc = dump_cnt;

  dump_reg();
  fprintf(stderr, "abr=0x%04x dbb=0x%04x ir=0x%02x (%s)\n",
	  m.ireg.abr, m.ireg.dbb, m.ireg.ir, 
	  inst_memonic(m.ireg.ir)); 

  dump_cnt++;
  return rc;
}
 
int 
dump_page(byte page)
{
  int rc = dump_cnt;
  byte offset;
  address addr=0;

  ADDR_SET_HIGH(addr, page);


  fprintf(stderr, "0x%04x:\n", addr);

  fprintf(stderr, "    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"); 
  fprintf(stderr, "    -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --\n"); 
  for (offset = 0; offset != 0xFF; offset++) {
    ADDR_SET_LOW(addr, offset);
    if ((offset & 0x0F) == 0x00) fprintf(stderr, "%02x: ", offset);
    // we avoid memory mapped devices and read memory array directly
    fprintf(stderr, "%02x", m.vs.ms.memory[addr]); 
    if ((offset & 0x0F) == 0x0F) fprintf(stderr, "\n");
    else fprintf(stderr, " ");
  }
  ADDR_SET_LOW(addr, offset);
  // we avoid memory mapped devices and read memory array directly
  fprintf(stderr, "%02x\n", m.vs.ms.memory[addr]);
  dump_cnt++;
  return rc;
}

int
dump_mem()
{
  int rc = dump_cnt;
  byte page;

  for (page = 0; page != 0xFF; page++) 
    dump_page( page);
  
  dump_page( page);

  dump_cnt++;
  return rc;
}

int
dump_machine(enum DumpMemDirective md)
{
  int rc = dump_cnt;

  dump_cpu();

  if (md==MEM_ALL) dump_mem();
  else if (md==MEM_SPECIAL) {
    dump_page(0x00);
    dump_page(0x01);
    dump_page(0xFF);
  }

  dump_cnt++;
  return rc;
}

int 
dump_am(struct AddressingModeDesc *am)
{
  int rc=dump_cnt;

  fprintf(stderr, "mode=%d, %s", am->mode,  am->desc);

  dump_cnt++;
  return rc;
}

int
dump_inst(struct InstructionDesc *i)
{
  int rc=dump_cnt;
  
  fprintf(stderr, "inst=%d, %s, %s", i->inst, i->memonic, i->description);

  dump_cnt++;
  return rc;
}

int
dump_op(opcode opc)
{
  struct OPCodeDesc *op = &OPCodeTable[opc];
  int rc=dump_cnt;

  if (!op) fprintf(stderr, "op=NULL\n");
  else {
    fprintf(stderr, "op=0x%02x am=%d\n(", op->op, op->am);
    dump_am(&(AMTable[op->am]));
    fprintf(stderr, ")\n ins=%d\n(",
	    op->ins);
    dump_inst(&InstTable[op->ins]);
    fprintf(stderr, ")\n");
  }
  dump_cnt++;
  return rc;
}

int 
misc_finalize(char *outfile) 
{
  int outmemfd;
  int rc = 0;

  if (outfile) {
    outmemfd = open(outfile, O_CREAT|O_TRUNC|O_WRONLY, 0666);
    if (outmemfd < 0) {
      fprintf(stderr, "ERROR opening %s\n", outfile);
      rc = -1;
    } else {
      rc = mem_img_save(outmemfd);
      close(outmemfd);
    }
  }

  trace_close(TraceTh);

  return rc;
}

int
misc_initialize(char *const infile, char *progfile,
                char *tracefile,
		int interrupt_signals,
		int load_mstate)
{
  int inmemfd;
  int rc;

  /* initialize TraceTh */
  if (trace_open(&TraceTh, tracefile, 1) < 0) {
    fprintf(stderr, "ERROR: %s: trace_open: %p, %s, 1\n",
	    __func__, &TraceTh, tracefile);
    return -1;
  }

  inmemfd = open(infile, O_RDONLY);
  if (inmemfd < 0) {
    fprintf(stderr, "ERROR opening %s\n", infile);
    return -1;
  }

  if (load_mstate) { // load machine state
    if (mstate_img_load(inmemfd) < 0) return -1;
  }
  else {             // load memory image
    if (mem_img_load(inmemfd) < 0) return -1;
  }

  // initialize memory mapping infrastructure
  if ((rc = mem_mappings_init()) < 0) return -1;

  if (!load_mstate && int_resb() < 0) rc = -1;
  
  // initialize console
  if (console_init(progfile) < 0) rc = -1;

  // initialize signals for interrupt support
  if (interrupt_signals) {
    if (int_init() < 0) rc = -1;
  }
  
  return rc;
}
