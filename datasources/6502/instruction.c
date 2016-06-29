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

#include "machine.h"
#include "mem.h"
#include "instruction.h"
#include "interrupt.h"
#include "misc.h"
#include "trace.h"


/* CODE: BEGIN */

#ifdef IMP
static inline void
setZN(byte v)
{
  if (v == 0) machine_sr_Z_SET();
  else        machine_sr_Z_CLR();

  if (v & BYTE_SIGN_MASK) machine_sr_N_SET();
  else                    machine_sr_N_CLR();
}
#endif

static inline int
inst_am_inv(void)
{
  NYI; assert(0); return 1;
}

static inline int
inst_am_acc(void)
{
#ifdef IMP
  // instruction has to figure it out 
  return 1;
#else 
  NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_abs(void)
{
#ifdef IMP
  address pc = machine_pc_get();
  ADDR_SET(m.ireg.abr,
	   mem_get(pc+1),  // set low byte of EA Base
	   mem_get(pc+2)); // set high byte of EA Base
  return 1;
#else
  NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_absx(void)
{
#ifdef IMP
  address pc = machine_pc_get();
  ADDR_SET(m.ireg.abr,
	   mem_get(pc+1),  // set low byte of EA Base
	   mem_get(pc+2)); // set high byte of EA Base

  m.ireg.abr += machine_x_get();              // unsigned addition
  return 1;
#else
 NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_absy(void)
{
#ifdef IMP
  address pc = machine_pc_get();
  ADDR_SET(m.ireg.abr,
	   mem_get(pc+1),  // set low byte of EA Base
	   mem_get(pc+2)); // set high byte of EA Base

  m.ireg.abr += machine_y_get();              // unsigned addition
  return 1;
#else
 NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_imm(void)
{
#ifdef IMP
  m.ireg.abr = machine_pc_get() + 1; // data is located in the next byte after the opcode 
  return 1;
#else
 NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_impl(void)
{
  // instruction knows what to do
  return 1;
}

static inline int
inst_am_ind(void)
{
#ifdef IMP
  address iaddr;

  // absolute indirect address

  ADDR_SET(iaddr, 
	   mem_get(machine_pc_get()+1),  // low byte from pc+1
	   mem_get(machine_pc_get()+2)); // high byte from pc+2

  ADDR_SET(m.ireg.abr,
	   mem_get(iaddr),    // set low byte of EA Base from iaddr
	   mem_get(iaddr+1)); // set high byte of EA Base from iaddr+1

  return 1;
#else
  NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_xind(void)
{
#ifdef IMP
  address iaddr;

  // indirect zp page address
  ADDR_SET(iaddr, 
	   (mem_get(machine_pc_get() + 1) + machine_x_get()),  // low byte from pc+1 plus x
	   0x00);                                              // zero page -> high 0x00

  ADDR_SET(m.ireg.abr, 
	   mem_get(iaddr),      // set low byte of EA Base from iaddr
	   mem_get(iaddr + 1)); // set high byte of EA Base from iaddr+1

  return 1;
#else
  NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_indy(void)
{
#ifdef IMP
  address iaddr;

  // indirect zp page address

  ADDR_SET(iaddr, 
	   mem_get(machine_pc_get() + 1),  // low byte from pc + 1 
	   0x00);                          // zero page -> high 0x00

  ADDR_SET(m.ireg.abr, 
	   mem_get(iaddr),      // set low byte of EA Base from iaddr
	   mem_get(iaddr + 1)); // set high byte of EA Base from iaddr+1

  m.ireg.abr += machine_y_get();                    // unsigned addition

  return 1;
#else
  NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_rel(void)
{
#ifdef IMP
  m.ireg.dbb = mem_get(machine_pc_get() + 1); // load offset
  m.ireg.abr = machine_pc_get() + 2;    // Base EA is PC of next instruction

  m.ireg.abr = ((short)(m.ireg.abr)) + ((char)(m.ireg.dbb));
  return 1;
#else
  NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_zp(void)
{
#ifdef IMP
  ADDR_SET(m.ireg.abr,
	   mem_get(machine_pc_get() + 1), // set low byte of EA Base
	   0x00);                      // set high byte of EA to Zero Page
  return 1;
#else
  NYI; assert(0); return 0;
#endif
}
 
static inline int
inst_am_zpx(void)
{
#ifdef IMP
  byte offset = mem_get(machine_pc_get() + 1) + machine_x_get();
  ADDR_SET(m.ireg.abr,
	   offset, // set low byte of EA Base
	   0x00);  // set high byte of EA to Zero Page
  return 1;
#else
  NYI; assert(0); return 0;
#endif
}

static inline int
inst_am_zpy(void)
{
#ifdef IMP
  byte offset = mem_get(machine_pc_get() + 1) + machine_y_get();
  ADDR_SET(m.ireg.abr,
	   offset, // set low byte of EA Base
	   0x00);  // set high byte of EA to Zero Page
  return 1;
#else
  NYI; assert(0); return 0;
#endif  
}

static inline int
inst_INV(void)
{ NYI; dump_op(machine_ir_get()); dump_reg();  assert(0); return 0; }

static inline int
inst_ADC(void)
{ 
#ifdef IMP
  unsigned short sresult; // result in larger size so that we don't loose bits 
                          // and can detect overflow of byte computation
  byte sv; // value to add to the ac
                     // larger size to accomodate carry
  byte oldac;  // copy of orginal accumulator value

  inst_load();

  // FIXME: validate with p14 of pgm manual (ensure signedness of comp is right)
  //  dump_op(machine_ir_get()); dump_reg();  assert(0); return 0;

  // we use local variable to make code more readable compile should optimize them away
  oldac =  machine_ac_get();
  sv = m.ireg.dbb; 
  // FORMULA: ac = ac + (M + C);
  // keep computing result into 16 bit size so that we can detect overflow
  sresult = oldac + sv + SR_C(machine_sr_get());

  // truncate the short result and put back into ac
  machine_ac_set((byte)sresult);

  // set flags 
  // set Zero and Negative flags in the standard way 
  setZN(machine_ac_get());

  // now take care of carry bit by looking at result in larger size
  if (sresult > 0xFF) machine_sr_C_SET();
  else                machine_sr_C_CLR();

  // now take care of overflow using the standard rule (I think that is what 
  // the 6502 is doing from the doc): overflow if - plus - == + || + plus + == - 
  if ( ((oldac   & BYTE_SIGN_MASK) == (sv       & BYTE_SIGN_MASK)) && 
       ((sresult & BYTE_SIGN_MASK) != (oldac    & BYTE_SIGN_MASK)) ) {
    machine_sr_V_SET(); 
  } else machine_sr_V_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_AND(void)
{ 
#ifdef IMP
  byte ac;

  inst_load();
  ac = machine_ac_get();
  ac &= m.ireg.dbb;
  machine_ac_set(ac);
  setZN(ac);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_ASL(void)
{ 
#ifdef IMP
  byte b,a;

  if (OPCodeTable[machine_ir_get()].am == ACC) {
    b = machine_ac_get();
    a = b << 1;
    machine_ac_set(a);
  } else {
    inst_load();
    b = m.ireg.dbb;
    a = m.ireg.dbb << 1;
    m.ireg.dbb = a;
    inst_store();
  }
  // set flags: Carry bit should be set to bit 7 of input 
  //            zero bit is set if result is zero or cleared othersize
  //            negative bit is set to bit 7 or result          
  if ((b & 0x80) == 0) machine_sr_C_CLR();
  else                 machine_sr_C_SET();

  if (a & 0x80) machine_sr_N_SET();
  else          machine_sr_N_CLR();

  if (a == 0) machine_sr_Z_SET();
  else        machine_sr_Z_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BCC(void)
{ 
#ifdef IMP
  if (SR_C(machine_sr_get()) == 0) machine_pc_set(m.ireg.abr);
  return 1;  
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BCS(void)
{ 
#ifdef IMP
  if (SR_C(machine_sr_get())) machine_pc_set(m.ireg.abr);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BEQ(void)
{ 
#ifdef IMP
  if (SR_Z(machine_sr_get())) machine_pc_set(m.ireg.abr);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BIT(void)
{ 
#ifdef IMP
  byte result;

  inst_load();
  result = m.ireg.dbb & machine_ac_get();

  if (result == 0) machine_sr_Z_SET();          // if result is zero set Z flag
  else             machine_sr_Z_CLR();

  if (m.ireg.dbb & 0x80) machine_sr_N_SET(); // set N Flag based on bit 7 of mem
  else                    machine_sr_N_CLR();

  if (m.ireg.dbb & 0x40) machine_sr_V_SET(); // set V Flag based on bit 6 of mem
  else                    machine_sr_V_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BMI(void)
{ 
#ifdef IMP
  if (SR_N(machine_sr_get())) machine_pc_set(m.ireg.abr);
  return 1;
#else  
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BNE(void)
{
#ifdef IMP
  if (SR_Z(machine_sr_get()) == 0) machine_pc_set(m.ireg.abr);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BPL(void)
{ 
#ifdef IMP
  if (SR_N(machine_sr_get()) == 0) machine_pc_set(m.ireg.abr);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BRK(void)
{ 
  NYI; dump_op(machine_ir_get()); dump_reg(); return -1;
}

static inline int
inst_BVC(void)
{
#ifdef IMP
  if (SR_V(machine_sr_get()) == 0) machine_pc_set(m.ireg.abr);
  return 1;  
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_BVS(void)
{
#ifdef IMP
  if (SR_V(machine_sr_get())) machine_pc_set(m.ireg.abr);
  return 1;  
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_CLC(void)
{ 
#ifdef IMP
  machine_sr_C_CLR();
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_CLD(void)
{ 
#ifdef IMP
  machine_sr_D_CLR();
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_CLI(void)
{ 
  machine_sr_I_CLR();
  return 1;
}

static inline int
inst_CLV(void)
{ 
#ifdef IMP
  machine_sr_V_CLR();
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_CMP(void)
{ 
#ifdef IMP
// not actually exercised yet
  int diff;
  byte ac, v;

  inst_load();
  
  ac = machine_ac_get(); v = m.ireg.dbb;
  diff = (int)((char)ac) - (int)((char)v);

  if (ac >= v) machine_sr_C_SET();
  else         machine_sr_C_CLR();

  if (diff == 0) machine_sr_Z_SET();
  else           machine_sr_Z_CLR();

  if (diff <  0) machine_sr_N_SET();
  else           machine_sr_N_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_CPX(void)
{ 
#ifdef IMP
  int diff;
  byte x, v;

  inst_load();
  
  x = machine_x_get(); v = m.ireg.dbb;
  diff = (int)((char)x) - (int)((char)v);

  if (x >= v) machine_sr_C_SET();
  else           machine_sr_C_CLR();

  if (diff == 0) machine_sr_Z_SET();
  else           machine_sr_Z_CLR();

  if (diff <  0) machine_sr_N_SET();
  else           machine_sr_N_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_CPY(void)
{
#ifdef IMP
  int diff;
  byte y, v;

  inst_load();
  
  y = machine_y_get(); v = m.ireg.dbb;
  diff = (int)((char)y) - (int)((char)v);

  if (y >= v) machine_sr_C_SET();
  else           machine_sr_C_CLR();

  if (diff == 0) machine_sr_Z_SET();
  else           machine_sr_Z_CLR();

  if (diff <  0) machine_sr_N_SET();
  else           machine_sr_N_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_DEC(void)
{ 
#ifdef IMP
  byte res;
  inst_load();
  m.ireg.dbb--;
  res = m.ireg.dbb;
  inst_store();
  setZN(res);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_DEX(void)
{ 
#ifdef IMP
  byte x = machine_x_get();
  x--;
  machine_x_set(x);
  setZN(x);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_DEY(void)
{
#ifdef IMP
  byte y = machine_y_get();
  y--;
  machine_y_set(y);
  setZN(y);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_EOR(void)
{ 
#ifdef IMP
  byte ac;
  inst_load();
  ac = machine_ac_get();
  ac ^= m.ireg.dbb;
  machine_ac_set(ac);
  setZN(ac);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_INC(void)
{ 
#ifdef IMP
  byte ac, res;

  if (OPCodeTable[machine_ir_get()].am == ACC) {
    ac = machine_ac_get();
    ac++;
    machine_ac_set(ac);
    res = ac;
  } else {
    inst_load();
    m.ireg.dbb++;
    res = m.ireg.dbb;
    inst_store();
  } 
  setZN(res);

  return 1;  
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_INX(void)
{
#ifdef IMP
  byte x = machine_x_get();
  x++;
  machine_x_set(x);
  setZN(x);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif

}

static inline int
inst_INY(void)
{ 
#ifdef IMP
  byte y = machine_y_get();
  y++;
  machine_y_set(y);
  setZN(y);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_JMP(void)
{ 
  machine_pc_set(m.ireg.abr);
  return 1;
}

static inline int
inst_JSR(void)
{ 
#ifdef IMP
  // quirk of 6502 jsr pushs addr of last byte of current PC
  // which at this point is -1 for the updated pc value
  address pc = machine_pc_get() - 1;
  inst_push16(pc);
  machine_pc_set(m.ireg.abr);
  return 1;
#else 
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_LDA(void)
{
#ifdef IMP
  inst_load();
  machine_ac_set(m.ireg.dbb);
  setZN(machine_ac_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_LDX(void)
{ 
#ifdef IMP
  inst_load();
  machine_x_set(m.ireg.dbb);
  setZN(machine_x_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_LDY(void)
{
#ifdef IMP
  inst_load();
  machine_y_set(m.ireg.dbb);
  setZN(machine_y_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_LSR(void)
{
#ifdef IMP
  byte b,a;

  if (OPCodeTable[machine_ir_get()].am == ACC) {
    b = machine_ac_get();
    a = b >> 1;
    machine_ac_set(a);
  } else {
    inst_load();
    b = m.ireg.dbb;
    a = m.ireg.dbb >> 1;
    m.ireg.dbb = a;
    inst_store();
  }
  // set flags: Carry bit should be set to low order bit value
  //            zero bit is set if result is zero or cleared othersize
  //            negative bit is set to 0          
  if ((b & 0x1) == 0) machine_sr_C_CLR();
  else                machine_sr_C_SET();

  if (a == 0) machine_sr_Z_SET();
  else      machine_sr_Z_CLR();

  machine_sr_N_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_NOP(void)
{ 
#ifdef IMP
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0; 
#endif
}

static inline int
inst_ORA(void)
{ 
#ifdef IMP
  byte ac;
  inst_load();
  ac = machine_ac_get();
  ac |= m.ireg.dbb;
  machine_ac_set(ac);
  setZN(ac);
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_PHA(void)
{ 
#ifdef IMP
  inst_push8(machine_ac_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_PHP(void)
{ 
#ifdef IMP
  inst_push8(machine_sr_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_PLA(void)
{ 
#ifdef IMP
  machine_ac_set(inst_pop());
  setZN(machine_ac_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_PLP(void)
{
#ifdef IMP
  machine_sr_set(inst_pop());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

// FIXME:  JA Not sure if any flags are adjusted if ROL or ROR are operating
//         on memory .... confirm what the right behavior is Manual is unclear:
// "The ROL instruction either shifts the accumulator left 1 bit and
//  stores the carry in accumulator bit 0 or does not affect the internal reg-
//  isters at all."
// 
// "The ROR instruction either shifts the accumulator right 1 bit and
//  stores the carry in accumulator bit 7 or does not affect the internal regis-
//  ters at all."

static inline int
inst_ROL(void)
{ 
#ifdef IMP
  byte b,a;
  byte sr = machine_sr_get();

  if (OPCodeTable[machine_ir_get()].am == ACC) {
    b = machine_ac_get();
    a = (b << 1) | SR_C(sr);        // lshift with carry into bit 0
    machine_ac_set(a);
  } else {
    inst_load();
    b = m.ireg.dbb;
    a = m.ireg.dbb << 1 | SR_C(sr); // lshift with carry into bit 0
    m.ireg.dbb = a;
    inst_store();
  }

  // set flags: Carry bit should be set to bit 7 of input value
  //            zero bit is set if result is zero or cleared othersize
  //            negative bit is set to bit 6 of input value         
  if ((b & 0x80) == 0) machine_sr_C_CLR();
  else                 machine_sr_C_SET();

  if ((b & 0x40) == 0) machine_sr_N_CLR();
  else                 machine_sr_N_SET();

  if (a == 0) machine_sr_Z_SET();
  else        machine_sr_Z_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_ROR(void)
{
#ifdef IMP
  byte b,a;
  byte sr = machine_sr_get();

  if (OPCodeTable[machine_ir_get()].am == ACC) {
    b = machine_ac_get();
    a = (b >> 1) | (SR_C(sr) << 7);  // rshift with carry into bit 7
    machine_ac_set(a);
  } else {
    inst_load();
    b = m.ireg.dbb;
    a = (b >> 1) | (SR_C(sr) << 7);  // rshift with carry into bit 7
    m.ireg.dbb = a;
    inst_store();
  }

  // set flags: Carry bit should be set to bit 0 of input value
  //            Zero bit is set if result is zero or cleared otherwise
  //            Negative bit is set to input carry

  // order matters here! must first N as it is based on input Carry bit value
  if (SR_C(sr) == 0) machine_sr_N_CLR();
  else               machine_sr_N_SET();

  if ((b & 0x01) == 0) machine_sr_C_CLR();
  else                 machine_sr_C_SET();

  if (a == 0) machine_sr_Z_SET();
  else        machine_sr_Z_CLR();
 
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_RTI(void)
{ 
  byte sr, pcl, pch;
  address pc;

  sr  = inst_pop();
  pcl = inst_pop();
  pch = inst_pop();

  machine_sr_set(sr);

  pc = machine_pc_get();
  ADDR_SET_LOW(pc, pcl);
  ADDR_SET_HIGH(pc, pch);
  machine_pc_set(pc);

  int_RTI();

  return 1;
}

static inline int
inst_RTS(void)
{ 
#ifdef IMP
  byte  pcl, pch;
  address pc = 0;

  pcl = inst_pop();
  pch = inst_pop();

  ADDR_SET_LOW(pc, pcl);
  ADDR_SET_HIGH(pc, pch);

  // quirk of 6502 JSR pushs address of byte before return address
  pc += 1;
  machine_pc_set(pc);

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_SBC(void)
{ 
#ifdef IMP
  unsigned short sresult; // result in larger size so that we don't loose bits 
                          // and can detect overflow of byte computation
  byte sv; // value to add to the ac
                     // larger size to accomodate carry
  byte oldac;  // copy of orginal accumulator value
  byte borrow; // ~carry
  
  inst_load();

  // FIXME: validate with p14 of pgm manual (ensure signedness of comp is right)
  //  dump_op(machine_ir_get()); dump_reg();  assert(0); return 0;

  // p 19 programmers manual
  // "It should be remembered that before using the SBC instruc-
  // tion, either signed or unsigned, the carry flag must be set to a 1 in
  // order to indicate a no borrow condition.  The resultant carry flag
  // has no meaning after a signed arithmetic operation."

  // we use local variable to make code more readable compile should optimize them away
  borrow = ~SR_C(machine_sr_get()) & 0x01;
  oldac =  machine_ac_get();
  sv = ~(m.ireg.dbb);
  // FORMULA: ac = ac + (~M + C)
  // keep computing result into 16 bit size so that we can detect overflow
  sresult = oldac + sv + SR_C(machine_sr_get());

  // truncate the short result and put back into ac
  machine_ac_set((byte)sresult);

  // set flags 
  // set Zero and Negative flags in the standard way 
  setZN(machine_ac_get());

  // now take care of carry (borrow) bit
  if (m.ireg.dbb + borrow > oldac) machine_sr_C_CLR();
  else                              machine_sr_C_SET();
  //printf("test: 0x%x + 0x%x = 0x%x > 0x%x\n", m.ireg.dbb, borrow, m.ireg.dbb + borrow, oldac);

  // now take care of overflow using the standard rule (I think that is what 
  // the 6502 is doing from the doc): overflow if - plus - == + || + plus + == - 
  if ( ((oldac   & BYTE_SIGN_MASK) == (sv       & BYTE_SIGN_MASK)) && 
       ((sresult & BYTE_SIGN_MASK) != (oldac    & BYTE_SIGN_MASK)) ) {
    machine_sr_V_SET(); 
  } else machine_sr_V_CLR();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg();  assert(0); return 0;
#endif
}

static inline int
inst_SEC(void)
{ 
#ifdef IMP
  machine_sr_C_SET();
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_SED(void)
{ 
  // if this ever gets invoked then we must implement
  // all decimal logic in relevant instructions eg. SDC, ADC, etc.
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
}

static inline int
inst_SEI(void)
{ NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;}

static inline int
inst_STA(void)
{
#ifdef IMP 
  // NO FLAGS AFFECTED
  m.ireg.dbb = machine_ac_get();
  inst_store();

  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif

}

static inline int
inst_STX(void)
{ 
#ifdef IMP
  // NO FLAGS AFFECTED
  m.ireg.dbb = machine_x_get();
  inst_store();
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_STY(void)
{
#ifdef IMP
  // NO FLAGS AFFECTED
  m.ireg.dbb = machine_y_get();
  inst_store();
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_TAX(void)
{
#ifdef IMP
  machine_x_set(machine_ac_get());
  setZN(machine_x_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_TAY(void)
{ 
#ifdef IMP
  machine_y_set(machine_ac_get());
  setZN(machine_y_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_TSX(void)
{ 
#ifdef IMP
  machine_x_set(machine_sp_get());
  setZN(machine_x_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_TXA(void)
{ 
#ifdef IMP
  machine_ac_set(machine_x_get());
  setZN(machine_ac_get());
  return 1;
#else  
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_TXS(void)
{
#ifdef IMP
  machine_sp_set(machine_x_get());
  return 1;
#else
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

static inline int
inst_TYA(void)
{ 
#ifdef IMP
  machine_ac_set(machine_y_get());
  setZN(machine_ac_get());
  return 1;
#else 
  NYI; dump_op(machine_ir_get()); dump_reg(); assert(0); return 0;
#endif
}

/* tables are now mainly just for debugging */
#include "amtbl.h"
#include "insttbl.h"
#include "optbl.h"

/* for TRACE_ACTION */
#include <string.h>

uint64_t
inst_loop(uint64_t count)
{
  int rc = 1;
  uint64_t i=0;
  address pc;
  
  while (1) {
    if (TraceState == TRACE_ON &&
	TraceType == TRACE_ACTION) {
      memset(m.ireg.actionRec, 0, ACTCNT * ACTIONSIZE);
      ACTCNT = 0;
    }
    
    interrupts();

    pc = machine_pc_get();
    machine_ir_set(mem_get(pc));

    switch (machine_ir_get()) {
    case 0x00:
      /* 0x00 IMPL BRK */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_BRK();
      break;
    case 0x01:
      /* 0x01 XIND ORA */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x02:
      /* 0x02 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x03:
      /* 0x03 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x04:
      /* 0x04 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x05:
      /* 0x05 ZP ORA */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x06:
      /* 0x06 ZP ASL */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_ASL();
      break;
    case 0x07:
      /* 0x07 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x08:
      /* 0x08 IMPL PHP */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_PHP();
      break;
    case 0x09:
      /* 0x09 IMM ORA */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x0a:
      /* 0x0a ACC ASL */
      inst_am_acc();
      pc += AM_ACC_BYTES;
      machine_pc_set(pc);
      rc = inst_ASL();
      break;
    case 0x0b:
      /* 0x0b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x0c:
      /* 0x0c INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x0d:
      /* 0x0d ABS ORA */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x0e:
      /* 0x0e ABS ASL */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_ASL();
      break;
    case 0x0f:
      /* 0x0f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x10:
      /* 0x10 REL BPL */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BPL();
      break;
    case 0x11:
      /* 0x11 INDY ORA */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x12:
      /* 0x12 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x13:
      /* 0x13 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x14:
      /* 0x14 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x15:
      /* 0x15 ZPX ORA */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x16:
      /* 0x16 ZPX ASL */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_ASL();
      break;
    case 0x17:
      /* 0x17 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x18:
      /* 0x18 IMPL CLC */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_CLC();
      break;
    case 0x19:
      /* 0x19 ABSY ORA */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x1a:
      /* 0x1a INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x1b:
      /* 0x1b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x1c:
      /* 0x1c INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x1d:
      /* 0x1d ABSX ORA */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_ORA();
      break;
    case 0x1e:
      /* 0x1e ABSX ASL */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_ASL();
      break;
    case 0x1f:
      /* 0x1f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x20:
      /* 0x20 ABS JSR */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_JSR();
      break;
    case 0x21:
      /* 0x21 XIND AND */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x22:
      /* 0x22 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x23:
      /* 0x23 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x24:
      /* 0x24 ZP BIT */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_BIT();
      break;
    case 0x25:
      /* 0x25 ZP AND */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x26:
      /* 0x26 ZP ROL */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_ROL();
      break;
    case 0x27:
      /* 0x27 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x28:
      /* 0x28 IMPL PLP */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_PLP();
      break;
    case 0x29:
      /* 0x29 IMM AND */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x2a:
      /* 0x2a ACC ROL */
      inst_am_acc();
      pc += AM_ACC_BYTES;
      machine_pc_set(pc);
      rc = inst_ROL();
      break;
    case 0x2b:
      /* 0x2b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x2c:
      /* 0x2c ABS BIT */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_BIT();
      break;
    case 0x2d:
      /* 0x2d ABS AND */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x2e:
      /* 0x2e ABS ROL */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_ROL();
      break;
    case 0x2f:
      /* 0x2f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x30:
      /* 0x30 REL BMI */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BMI();
      break;
    case 0x31:
      /* 0x31 INDY AND */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x32:
      /* 0x32 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x33:
      /* 0x33 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x34:
      /* 0x34 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x35:
      /* 0x35 ZPX AND */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x36:
      /* 0x36 ZPX ROL */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_ROL();
      break;
    case 0x37:
      /* 0x37 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x38:
      /* 0x38 IMPL SEC */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_SEC();
      break;
    case 0x39:
      /* 0x39 ABSY AND */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x3a:
      /* 0x3a INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x3b:
      /* 0x3b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x3c:
      /* 0x3c INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x3d:
      /* 0x3d ABSX AND */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_AND();
      break;
    case 0x3e:
      /* 0x3e ABSX ROL */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_ROL();
      break;
    case 0x3f:
      /* 0x3f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x40:
      /* 0x40 IMPL RTI */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_RTI();
      break;
    case 0x41:
      /* 0x41 XIND EOR */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x42:
      /* 0x42 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x43:
      /* 0x43 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x44:
      /* 0x44 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x45:
      /* 0x45 ZP EOR */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x46:
      /* 0x46 ZP LSR */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_LSR();
      break;
    case 0x47:
      /* 0x47 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x48:
      /* 0x48 IMPL PHA */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_PHA();
      break;
    case 0x49:
      /* 0x49 IMM EOR */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x4a:
      /* 0x4a ACC LSR */
      inst_am_acc();
      pc += AM_ACC_BYTES;
      machine_pc_set(pc);
      rc = inst_LSR();
      break;
    case 0x4b:
      /* 0x4b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x4c:
      /* 0x4c ABS JMP */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_JMP();
      break;
    case 0x4d:
      /* 0x4d ABS EOR */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x4e:
      /* 0x4e ABS LSR */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_LSR();
      break;
    case 0x4f:
      /* 0x4f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x50:
      /* 0x50 REL BVC */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BVC();
      break;
    case 0x51:
      /* 0x51 INDY EOR */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x52:
      /* 0x52 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x53:
      /* 0x53 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x54:
      /* 0x54 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x55:
      /* 0x55 ZPX EOR */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x56:
      /* 0x56 ZPX LSR */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_LSR();
      break;
    case 0x57:
      /* 0x57 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x58:
      /* 0x58 IMPL CLI */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_CLI();
      break;
    case 0x59:
      /* 0x59 ABSY EOR */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x5a:
      /* 0x5a INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x5b:
      /* 0x5b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x5c:
      /* 0x5c INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x5d:
      /* 0x5d ABSX EOR */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_EOR();
      break;
    case 0x5e:
      /* 0x5e ABSX LSR */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_LSR();
      break;
    case 0x5f:
      /* 0x5f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x60:
      /* 0x60 IMPL RTS */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_RTS();
      break;
    case 0x61:
      /* 0x61 XIND ADC */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x62:
      /* 0x62 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x63:
      /* 0x63 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x64:
      /* 0x64 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x65:
      /* 0x65 ZP ADC */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x66:
      /* 0x66 ZP ROR */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_ROR();
      break;
    case 0x67:
      /* 0x67 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x68:
      /* 0x68 IMPL PLA */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_PLA();
      break;
    case 0x69:
      /* 0x69 IMM ADC */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x6a:
      /* 0x6a ACC ROR */
      inst_am_acc();
      pc += AM_ACC_BYTES;
      machine_pc_set(pc);
      rc = inst_ROR();
      break;
    case 0x6b:
      /* 0x6b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x6c:
      /* 0x6c IND JMP */
      inst_am_ind();
      pc += AM_IND_BYTES;
      machine_pc_set(pc);
      rc = inst_JMP();
      break;
    case 0x6d:
      /* 0x6d ABS ADC */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x6e:
      /* 0x6e ABS ROR */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_ROR();
      break;
    case 0x6f:
      /* 0x6f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x70:
      /* 0x70 REL BVS */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BVS();
      break;
    case 0x71:
      /* 0x71 INDY ADC */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x72:
      /* 0x72 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x73:
      /* 0x73 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x74:
      /* 0x74 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x75:
      /* 0x75 ZPX ADC */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x76:
      /* 0x76 ZPX ROR */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_ROR();
      break;
    case 0x77:
      /* 0x77 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x78:
      /* 0x78 IMPL SEI */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_SEI();
      break;
    case 0x79:
      /* 0x79 ABSY ADC */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x7a:
      /* 0x7a INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x7b:
      /* 0x7b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x7c:
      /* 0x7c INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x7d:
      /* 0x7d ABSX ADC */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_ADC();
      break;
    case 0x7e:
      /* 0x7e ABSX ROR */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_ROR();
      break;
    case 0x7f:
      /* 0x7f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x80:
      /* 0x80 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x81:
      /* 0x81 XIND STA */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_STA();
      break;
    case 0x82:
      /* 0x82 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x83:
      /* 0x83 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x84:
      /* 0x84 ZP STY */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_STY();
      break;
    case 0x85:
      /* 0x85 ZP STA */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_STA();
      break;
    case 0x86:
      /* 0x86 ZP STX */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_STX();
      break;
    case 0x87:
      /* 0x87 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x88:
      /* 0x88 IMPL DEY */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_DEY();
      break;
    case 0x89:
      /* 0x89 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x8a:
      /* 0x8a IMPL TXA */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_TXA();
      break;
    case 0x8b:
      /* 0x8b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x8c:
      /* 0x8c ABS STY */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_STY();
      break;
    case 0x8d:
      /* 0x8d ABS STA */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_STA();
      break;
    case 0x8e:
      /* 0x8e ABS STX */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_STX();
      break;
    case 0x8f:
      /* 0x8f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x90:
      /* 0x90 REL BCC */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BCC();
      break;
    case 0x91:
      /* 0x91 INDY STA */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_STA();
      break;
    case 0x92:
      /* 0x92 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x93:
      /* 0x93 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x94:
      /* 0x94 ZPX STY */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_STY();
      break;
    case 0x95:
      /* 0x95 ZPX STA */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_STA();
      break;
    case 0x96:
      /* 0x96 ZPY STX */
      inst_am_zpy();
      pc += AM_ZPY_BYTES;
      machine_pc_set(pc);
      rc = inst_STX();
      break;
    case 0x97:
      /* 0x97 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x98:
      /* 0x98 IMPL TYA */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_TYA();
      break;
    case 0x99:
      /* 0x99 ABSY STA */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_STA();
      break;
    case 0x9a:
      /* 0x9a IMPL TXS */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_TXS();
      break;
    case 0x9b:
      /* 0x9b INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x9c:
      /* 0x9c INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x9d:
      /* 0x9d ABSX STA */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_STA();
      break;
    case 0x9e:
      /* 0x9e INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0x9f:
      /* 0x9f INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xa0:
      /* 0xa0 IMM LDY */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_LDY();
      break;
    case 0xa1:
      /* 0xa1 XIND LDA */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xa2:
      /* 0xa2 IMM LDX */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_LDX();
      break;
    case 0xa3:
      /* 0xa3 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xa4:
      /* 0xa4 ZP LDY */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_LDY();
      break;
    case 0xa5:
      /* 0xa5 ZP LDA */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xa6:
      /* 0xa6 ZP LDX */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_LDX();
      break;
    case 0xa7:
      /* 0xa7 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xa8:
      /* 0xa8 IMPL TAY */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_TAY();
      break;
    case 0xa9:
      /* 0xa9 IMM LDA */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xaa:
      /* 0xaa IMPL TAX */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_TAX();
      break;
    case 0xab:
      /* 0xab INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xac:
      /* 0xac ABS LDY */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_LDY();
      break;
    case 0xad:
      /* 0xad ABS LDA */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xae:
      /* 0xae ABS LDX */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_LDX();
      break;
    case 0xaf:
      /* 0xaf INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xb0:
      /* 0xb0 REL BCS */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BCS();
      break;
    case 0xb1:
      /* 0xb1 INDY LDA */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xb2:
      /* 0xb2 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xb3:
      /* 0xb3 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xb4:
      /* 0xb4 ZPX LDY */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_LDY();
      break;
    case 0xb5:
      /* 0xb5 ZPX LDA */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xb6:
      /* 0xb6 ZPY LDX */
      inst_am_zpy();
      pc += AM_ZPY_BYTES;
      machine_pc_set(pc);
      rc = inst_LDX();
      break;
    case 0xb7:
      /* 0xb7 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xb8:
      /* 0xb8 IMPL CLV */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_CLV();
      break;
    case 0xb9:
      /* 0xb9 ABSY LDA */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xba:
      /* 0xba IMPL TSX */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_TSX();
      break;
    case 0xbb:
      /* 0xbb INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xbc:
      /* 0xbc ABSX LDY */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_LDY();
      break;
    case 0xbd:
      /* 0xbd ABSX LDA */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_LDA();
      break;
    case 0xbe:
      /* 0xbe ABSY LDX */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_LDX();
      break;
    case 0xbf:
      /* 0xbf INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xc0:
      /* 0xc0 IMM CPY */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_CPY();
      break;
    case 0xc1:
      /* 0xc1 XIND CMP */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xc2:
      /* 0xc2 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xc3:
      /* 0xc3 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xc4:
      /* 0xc4 ZP CPY */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_CPY();
      break;
    case 0xc5:
      /* 0xc5 ZP CMP */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xc6:
      /* 0xc6 ZP DEC */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_DEC();
      break;
    case 0xc7:
      /* 0xc7 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xc8:
      /* 0xc8 IMPL INY */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_INY();
      break;
    case 0xc9:
      /* 0xc9 IMM CMP */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xca:
      /* 0xca IMPL DEX */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_DEX();
      break;
    case 0xcb:
      /* 0xcb INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xcc:
      /* 0xcc ABS CPY */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_CPY();
      break;
    case 0xcd:
      /* 0xcd ABS CMP */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xce:
      /* 0xce ABS DEC */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_DEC();
      break;
    case 0xcf:
      /* 0xcf INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xd0:
      /* 0xd0 REL BNE */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BNE();
      break;
    case 0xd1:
      /* 0xd1 INDY CMP */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xd2:
      /* 0xd2 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xd3:
      /* 0xd3 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xd4:
      /* 0xd4 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xd5:
      /* 0xd5 ZPX CMP */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xd6:
      /* 0xd6 ZPX DEC */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_DEC();
      break;
    case 0xd7:
      /* 0xd7 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xd8:
      /* 0xd8 IMPL CLD */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_CLD();
      break;
    case 0xd9:
      /* 0xd9 ABSY CMP */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xda:
      /* 0xda INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xdb:
      /* 0xdb INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xdc:
      /* 0xdc INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xdd:
      /* 0xdd ABSX CMP */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_CMP();
      break;
    case 0xde:
      /* 0xde ABSX DEC */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_DEC();
      break;
    case 0xdf:
      /* 0xdf INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xe0:
      /* 0xe0 IMM CPX */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_CPX();
      break;
    case 0xe1:
      /* 0xe1 XIND SBC */
      inst_am_xind();
      pc += AM_XIND_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xe2:
      /* 0xe2 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xe3:
      /* 0xe3 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xe4:
      /* 0xe4 ZP CPX */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_CPX();
      break;
    case 0xe5:
      /* 0xe5 ZP SBC */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xe6:
      /* 0xe6 ZP INC */
      inst_am_zp();
      pc += AM_ZP_BYTES;
      machine_pc_set(pc);
      rc = inst_INC();
      break;
    case 0xe7:
      /* 0xe7 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xe8:
      /* 0xe8 IMPL INX */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_INX();
      break;
    case 0xe9:
      /* 0xe9 IMM SBC */
      inst_am_imm();
      pc += AM_IMM_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xea:
      /* 0xea IMPL NOP */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_NOP();
      break;
    case 0xeb:
      /* 0xeb INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xec:
      /* 0xec ABS CPX */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_CPX();
      break;
    case 0xed:
      /* 0xed ABS SBC */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xee:
      /* 0xee ABS INC */
      inst_am_abs();
      pc += AM_ABS_BYTES;
      machine_pc_set(pc);
      rc = inst_INC();
      break;
    case 0xef:
      /* 0xef INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xf0:
      /* 0xf0 REL BEQ */
      inst_am_rel();
      pc += AM_REL_BYTES;
      machine_pc_set(pc);
      rc = inst_BEQ();
      break;
    case 0xf1:
      /* 0xf1 INDY SBC */
      inst_am_indy();
      pc += AM_INDY_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xf2:
      /* 0xf2 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xf3:
      /* 0xf3 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xf4:
      /* 0xf4 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xf5:
      /* 0xf5 ZPX SBC */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xf6:
      /* 0xf6 ZPX INC */
      inst_am_zpx();
      pc += AM_ZPX_BYTES;
      machine_pc_set(pc);
      rc = inst_INC();
      break;
    case 0xf7:
      /* 0xf7 INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xf8:
      /* 0xf8 IMPL SED */
      inst_am_impl();
      pc += AM_IMPL_BYTES;
      machine_pc_set(pc);
      rc = inst_SED();
      break;
    case 0xf9:
      /* 0xf9 ABSY SBC */
      inst_am_absy();
      pc += AM_ABSY_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xfa:
      /* 0xfa INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xfb:
      /* 0xfb INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xfc:
      /* 0xfc INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    case 0xfd:
      /* 0xfd ABSX SBC */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_SBC();
      break;
    case 0xfe:
      /* 0xfe ABSX INC */
      inst_am_absx();
      pc += AM_ABSX_BYTES;
      machine_pc_set(pc);
      rc = inst_INC();
      break;
    case 0xff:
      /* 0xff INVALIDAM INV */
      inst_am_inv();
      pc += AM_INVALIDAM_BYTES;
      machine_pc_set(pc);
      rc = inst_INV();
      break;
    default:
      assert(0);
    }

#if 0
    // cause big drop in performance on my laptop when enabled (~78MIPS -> ~50MIPS)
    annotation_process();
#endif
#if 1
    // cause big drop in performance on my laptop when enabled (~78MIPS -> ~40MIPS)
    trace();
#endif
    TRACE_LOOP(dump_cpu());

    i++;
    if (rc < 0 || (count && i == count)) break;
  }
  return i;
}
