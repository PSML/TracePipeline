#ifndef __MACHINE_H__
#define __MACHINE_H__
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
// Derived based on information from following places
// http://archive.6502.org/books/mcs6500_family_programming_manual.pdf
// http://archive.6502.org/books/mcs6500_family_hardware_manual.pdf
// http://archive.6502.org/datasheets/mos_6500_mpu_nov_1985.pdf
// http://archive.6502.org/datasheets/wdc_w65c02s_oct_19_2010.pdf
// http://www.masswerk.at/6502/6502_instruction_set.html
// http://www.6502.org/tutorials/6502opcodes.html
// http://nesdev.parodius.com/6502.txt
// http://www.llx.com/~nparker/a2/index.html
// http://www.oxyron.de/html/opcodes02.html
//
// Nice summary of the 6502 can be found here:
// http://www.cpu-world.com/CPUs/650x/index.html
// http://www.cpu-world.com/Arch/650x.html
//
// NOTES:
// FROM: http://www.masswerk.at/6502/6502_instruction_set.html
// "Processor Stack:
//    LIFO, top down, 8 bit range, 0x0100 - 0x01FF 
//  Bytes, Words, Addressing:
//    8 bit bytes, 16 bit words in lobyte-hibyte representation (Little-Endian).
//   16 bit address range, operands follow instruction codes."

#include <inttypes.h>
#include <string.h>

typedef unsigned char  byte;
typedef unsigned short address;
typedef byte           instruction;

#define ADDR_LOW(addr)         ((byte)(0xFF & (addr)))
#define ADDR_HIGH(addr)        ((byte)((0xFF00 & (addr)) >> 8))
#define ADDR_PAGE(addr)        ADDR_HIGH(addr)
#define ADDR_SET_LOW(addr, b)  (addr = (((addr) & 0xFF00) | (address)(b)))
#define ADDR_SET_HIGH(addr, b) (addr = (((addr) & 0x00FF) | (((address)(b)) << 8)))
#define ADDR_SET(addr, lb, hb) (addr = (hb << 8) | lb)

#define BYTE_SIGN_BIT  7
#define BYTE_SIGN_MASK (1 << BYTE_SIGN_BIT)

#define STACK_PAGE 0x01

#define REG(member) (m.vs.ms.reg.data.member)

// programmer visible registers
union registers {
  uint64_t raw;
  struct {
    address       pc;  // Program counter
    byte          ac;  // Accumulator A
    byte          y;   // Index Register Y
    byte          x;   // Index Register X
    byte          sp;  // Stack pointer (0x01<SP>-0x01<SP>)
    byte          sr;  // (P)rocessor Status Register
                       // broken down into the following bit fields
                       // bit 7: N Negative/sign 1 = neg
                       // bit 6: V Overflow      1 = true
                       // bit 5: ignored/reserved (unspecifed value)
                       // bit 4: BRK Command,    1 = BRK, 0 = IRQB
                       // bit 3: Decimal mode    1 = true
                       // bit 2: IRQB disable    1 = disable
                       // bit 1: Zero            1 = true
                       // bit 0: Carry           1 = true
    union {            
      byte  padding;
      byte  traceIR;
    } extra;           // used for padding and to allow  
                       // extra data to be hidden in the         
                       // register structures.  NOTE:             
                       // Internal use only. 	              
                       // registers of machine structure         
                       // should not manipulate this             
                       // as the state vector will leak          
                       // internal state.
  } data;
};

/*
#define SR_CTR_ER(er) (er.data.memAccess.data.srCtr)
#define SR_CTR(m)     (SR_CTR_ER(m.ireg.execRec))

#define SR_N_CTR      128
#define SR_V_CTR       64
#define SR_B_CTR       16
#define SR_D_CTR        8
#define SR_I_CTR        4
#define SR_Z_CTR        2
#define SR_C_CTR        1
*/


#define pc_ACTION       0
#define ac_ACTION       1
#define y_ACTION        2
#define x_ACTION        3
#define sp_ACTION       4
#define sr_ACTION       5

extern char *ActionName[];

#define ACTION(i) (m.ireg.actionRec[i])
#define ACTCNT    (m.ireg.actionCnt)
#define MAX_DATA_ACCESSES 255

typedef address psml_address_t;
#define PSML_VALSIZE (sizeof(address))

#include "../compress/traceformat.h"

struct iregisters {
  // internal registers used by cpu operation
  address      abr;     // address bus register
  byte         dbb;     // data bus buffer
  instruction  ir;      // instruction register
  struct DecodedInfo {
    void   *v1; // decoded value
    void   *v2;
  } decoded; 
  byte         pendingNMI;
  byte         runningNMI;
  byte         pendingIRQ;
  byte         pendingBRK;
  action_t actionRec[MAX_DATA_ACCESSES];
  uint8_t  actionCnt;
};

#define MEMSIZE (1 << (sizeof(address) * 8))

struct mstate {
  union registers reg;
  byte  memory[MEMSIZE];
};

union vstate {
  unsigned char raw[sizeof(struct mstate)];
  struct mstate ms;
};

struct machine {
  union vstate vs;
  struct iregisters ireg;
};

extern struct machine m;


#include "trace.h"

#ifdef __DEBUG__
uint8_t prev_sv[sizeof(m.vs.raw)];
#endif

inline static byte
machine_ir_get(void)
{
  return m.ireg.ir;
}

inline static void
machine_ir_set(byte v)
{
  m.ireg.ir = v;
#ifdef TRACEIR   
  ERTRACE_IR() = v;
  SVTRACE_IR() = v;
#endif
}

#define DEFINE_REG_FUNCS(REG, TYPE)			\
  static inline TYPE					\
  machine_ ## REG ## _get() {				\
    if (TraceState == TRACE_ON && TraceType == TRACE_ACTION) { \
      ACTION(ACTCNT).hdr.id  = T_REG_RD;		\
      ACTION(ACTCNT).hdr.len = sizeof(TYPE);		\
      ACTION(ACTCNT).addr = REG ## _ACTION;		\
      *((TYPE *)ACTION(ACTCNT).val) = m.vs.ms.reg.data.REG;		\
      ACTCNT++;						\
    }							\
    return m.vs.ms.reg.data.REG;			\
  }							\
  static inline void					\
  machine_ ## REG ## _set(TYPE v) {			\
    m.vs.ms.reg.data.REG = v;				\
    if (TraceState == TRACE_ON && TraceType == TRACE_ACTION) { \
      ACTION(ACTCNT).hdr.id  = T_REG_WR;	        \
      ACTION(ACTCNT).hdr.len = sizeof(TYPE);	        \
      ACTION(ACTCNT).addr = REG ## _ACTION;		\
      *((TYPE *)ACTION(ACTCNT).val) = m.vs.ms.reg.data.REG;	\
      ACTCNT++;						\
    }							\
  }
DEFINE_REG_FUNCS(pc, address);
DEFINE_REG_FUNCS(ac, byte);
DEFINE_REG_FUNCS(y,  byte);
DEFINE_REG_FUNCS(x,  byte);
DEFINE_REG_FUNCS(sp, byte);
DEFINE_REG_FUNCS(sr, byte);

#define SR_N_BIT   7
#define SR_V_BIT   6
#define SR_B_BIT   4  // next bit in SR is 1
#define SR_D_BIT   3
#define SR_I_BIT   2
#define SR_Z_BIT   1
#define SR_C_BIT   0

#define SR_MASK(srbit) (1 << SR_ ## srbit ## _BIT)

#define SR_N_MASK SR_MASK(N)
#define SR_V_MASK SR_MASK(V)
#define SR_B_MASK SR_MASK(B)
#define SR_D_MASK SR_MASK(D)
#define SR_I_MASK SR_MASK(I)
#define SR_Z_MASK SR_MASK(Z)
#define SR_C_MASK SR_MASK(C)

#define SR_N(sr)        ((sr & SR_N_MASK) != 0)
#define SR_N_SET(sr, v) (v = sr |  SR_N_MASK)
#define SR_N_CLR(sr, v) (v = sr & ~SR_N_MASK)

#define SR_V(sr)        ((sr & SR_V_MASK) != 0)
#define SR_V_SET(sr, v) (v = sr |  SR_V_MASK)
#define SR_V_CLR(sr, v) (v = sr & ~SR_V_MASK)

#define SR_B(sr)        ((sr & SR_B_MASK) != 0)
#define SR_B_SET(sr, v) (v = sr |  SR_B_MASK)
#define SR_B_CLR(sr, v) (v = sr & ~SR_B_MASK)

#define SR_D(sr)        ((sr & SR_D_MASK) != 0)
#define SR_D_SET(sr, v) (v = sr |  SR_D_MASK)
#define SR_D_CLR(sr, v) (v = sr & ~SR_D_MASK)

#define SR_I(sr)        ((sr & SR_I_MASK) != 0)
#define SR_I_SET(sr, v) (v = sr |  SR_I_MASK)
#define SR_I_CLR(sr, v) (v = sr & ~SR_I_MASK)

#define SR_Z(sr)        ((sr & SR_Z_MASK) != 0)
#define SR_Z_SET(sr, v) (v = sr |  SR_Z_MASK)
#define SR_Z_CLR(sr, v) (v = sr & ~SR_Z_MASK)

#define SR_C(sr)        ((sr & SR_C_MASK) != 0)
#define SR_C_SET(sr, v) (v = sr |  SR_C_MASK)
#define SR_C_CLR(sr, v) (v = sr & ~SR_C_MASK)

#define DEFINE_SRBIT_FUNCS(FLAG, OP)	\
  static inline void			\
  machine_sr_ ## FLAG ## _ ## OP()	\
  {					\
    byte v;				\
    byte sr = machine_sr_get();		\
    SR_ ## FLAG ## _ ## OP(sr, v);	\
    machine_sr_set(v);			\
  }

DEFINE_SRBIT_FUNCS(N, SET);
DEFINE_SRBIT_FUNCS(N, CLR);
DEFINE_SRBIT_FUNCS(V, SET);
DEFINE_SRBIT_FUNCS(V, CLR);
DEFINE_SRBIT_FUNCS(B, SET);
DEFINE_SRBIT_FUNCS(B, CLR);
DEFINE_SRBIT_FUNCS(D, SET);
DEFINE_SRBIT_FUNCS(D, CLR);
DEFINE_SRBIT_FUNCS(I, SET);
DEFINE_SRBIT_FUNCS(I, CLR);
DEFINE_SRBIT_FUNCS(Z, SET);
DEFINE_SRBIT_FUNCS(Z, CLR);
DEFINE_SRBIT_FUNCS(C, SET);
DEFINE_SRBIT_FUNCS(C, CLR);

#define VEC_NMIB_LOW  0xFFFA
#define VEC_NMIB_HIGH 0xFFFB
#define VEC_RESB_LOW  0xFFFC
#define VEC_RESB_HIGH 0xFFFD
#define VEC_BRK_LOW   0xFFFE
#define VEC_BRK_HIGH  0xFFFF
#define VEC_IRQB_LOW  VEC_BRK_LOW
#define VEC_IRQB_HIGH VEC_BRK_HIGH

#endif
