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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include "machine.h"
#include "mem.h"
#include "interrupt.h"
#include "instruction.h"
#include "misc.h"

void
sighandler(int sig, siginfo_t *siginfo, void *context)
{
  if (sig == SIGUSR1) m.ireg.pendingNMI = 1;
  else if (sig == SIGUSR2) m.ireg.pendingIRQ = 1;
}

int 
int_init(void)
{
  struct sigaction setmask;
  
  sigemptyset( &setmask.sa_mask );
  
  setmask.sa_sigaction = sighandler;
  setmask.sa_flags     = 0;
  
  sigaction( SIGUSR1, &setmask, NULL );
  sigaction( SIGUSR2, &setmask, NULL );	 
  
  return 1;
}


int
int_resb(void)
{
  address pc;

  bzero((void *)&(m.vs.ms.reg), sizeof(m.vs.ms.reg));
  bzero((void *)&(m.ireg), sizeof(m.ireg));

  // values explicit set by hardware on reset
  machine_sr_B_SET();   
  machine_sr_D_CLR();
  machine_sr_I_SET();

  ADDR_SET(pc, mem_get(VEC_RESB_LOW), mem_get(VEC_RESB_HIGH));
  machine_pc_set(pc);

  TRACE_INT(fprintf(stderr, "RESET:\n")); 
  TRACE_INT(dump_machine(MEM_SPECIAL));
  return 1;
}

void
pushstate(void)
{
  byte pc_sr[3];
  pc_sr[0] = ADDR_HIGH(machine_pc_get());
  pc_sr[1] = ADDR_LOW(machine_pc_get());
  pc_sr[2] = machine_sr_get();

  inst_push24(pc_sr);
}

void
int_RTI(void)
{
  if (m.ireg.runningNMI==1) m.ireg.runningNMI=0;
}

// these functions get executed at top of loop
// as such pc is current set to next instruction to be executed
int
int_nmib(void)
{
  address pc;

  // we explicit ensure that only one NMI is executing at once
  if (m.ireg.runningNMI==1) return 0;
  m.ireg.runningNMI=1;
  pushstate();

  ADDR_SET(pc, mem_get(VEC_NMIB_LOW), mem_get(VEC_NMIB_HIGH));
  machine_pc_set(pc);

  machine_sr_I_SET();
  TRACE_INT(fprintf(stderr, "NMI: "));
  TRACE_INT(dump_machine(MEM_SPECIAL));
  return 1;
}

int
int_irqb(void)
{
  address pc;

  pushstate();

  ADDR_SET(pc, mem_get(VEC_IRQB_LOW), mem_get(VEC_IRQB_HIGH));
  machine_pc_set(pc);

  machine_sr_I_SET();
  TRACE_INT(fprintf(stderr, "IRQ: "));
  TRACE_INT(dump_machine(MEM_SPECIAL));
  return 1;
}

int
int_brkb(void)
{
  NYI;
  VPRINT("BRK: %d\n", dump_reg());
  return 1;
}
