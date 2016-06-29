#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__
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

// Following info from http://www.masswerk.at/6502/6502_instruction_set.html
/* Address Modes: */
/* A		Accumulator	 	OPC A	 	operand is AC */
/* abs		absolute	 	OPC $HHLL	operand is address $HHLL */
/* abs,X	absolute, X-indexed	OPC $HHLL,X	operand is address incremented 
                                                        by X with carry */
/* abs,Y	absolute, Y-indexed	OPC $HHLL,Y	operand is address incremented 
                                                        by Y with carry */
/* #		immediate	 	OPC #$BB	operand is byte (BB) */
/* impl		implied	        	OPC	 	operand implied */
/* ind		indirect	 	OPC ($HHLL)	operand is effective address;
                                                        effective address is value of address */
/* X,ind	X-indexed, indirect	OPC ($BB,X)	operand is effective zeropage address;
                                                        effective address is byte (BB) 
                                                        incremented by X without carry */
/* ind,Y	indirect, Y-indexed     OPC ($LL),Y	operand is effective address incremented 
                                                        by Y with carry; 
                                                        effective address is word at zeropage 
                                                        address */
/* rel		relative	 	OPC $BB	 	branch target is PC + offset (BB), 
                                                        bit 7 signifies negative offset */
/* zpg		zeropage	 	OPC $LL	 	operand is of address; 
                                                        address hibyte = zero ($00xx) */
/* zpg,X	zeropage, X-indexed	OPC $LL,X       operand is address incremented by X; 
                                                        address hibyte = zero ($00xx); 
                                                        no page transition */
/* zpg,Y	zeropage, Y-indexed	OPC $LL,Y	operand is address incremented by Y; 
                                                        address hibyte = zero ($00xx); 
                                                        no page transition */
enum  AddressingMode {
  INVALIDAM=0, // SYNTHETIC INVALID ADDRESSING MODE
  ACC,         // A: Accumulator A
  ABS,         // abs: Absolute a 
  ABSX,        // abs,X: Absolute Indexed with X a,x
  ABSY,        // abs,Y: Absolute Indexed with Y a,y
  IMM,         // #: Immediate #
  IMPL,        // impl: Implied i
  IND,         // ind: Absolute Indirect (a)
  XIND,        // X,ind: Zero Page Index Indirect (zp,x)
  INDY,        // ind,Y: Zero Page Indirect Indexed with Y (zp),y
  REL,         // rel: Program Counter Relative r
  ZP,          // zpg: Zero Page zp
  ZPX,         // zpg,X: Zero Page Index with X
  ZPY,         // zpg,Y: Zero Page Index with Y
};

struct AddressingModeDesc;

struct AddressingModeDesc {
  enum   AddressingMode mode;
  char   bytes; // number of bytes this address mode format 
                // ocupies, including the opcode itself.
                // pc plus this number of bytes get you to the 
                // next opcode
  char   desc[80];
  char   prfmt[20];
}; 

extern struct AddressingModeDesc AMTable[];

enum Instruction {
  INV, 
  ADC, AND, ASL, 
  BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, 
  CLC, CLD, CLI, CLV, CMP, CPX, CPY, 
  DEC, DEX, DEY, 
  EOR, 
  INC, INX, INY, 
  JMP, JSR, 
  LDA, LDX, LDY, LSR, 
  NOP, 
  ORA, 
  PHA, PHP, PLA, PLP, 
  ROL, ROR, RTI, RTS, 
  SBC, SEC, SED, SEI, STA, STX, STY, 
  TAX, TAY, TSX, TXA, TXS, TYA 
};

struct InstructionDesc {
  enum Instruction inst;
  char memonic[4];
  char description[80];
};

extern struct InstructionDesc InstTable[];

#define NUMOPCODES 256 
typedef byte opcode;
struct OPCodeDesc;

// I HAVE CHOOSEN TO USE 0x02 AS THE INVALID OPCODE SIGNATURE
// WHICH IS THE FIRST UNUSED OPCODE THAT I COULD FIND THAT SHOULD
// BE TREATED AS KIL OPCODE (see http://www.pagetable.com/?p=39)
// REQUIRING A PROCESSOR RESET
#define INVALIDOPCODE 0x02

struct OPCodeDesc {
  opcode op;
  enum AddressingMode am;
  enum Instruction ins;
};

extern struct OPCodeDesc OPCodeTable[NUMOPCODES];

static inline char *
inst_memonic(opcode opc)
{ 
  return InstTable[OPCodeTable[opc].ins].memonic; 
}

static inline struct AddressingModeDesc *
inst_addressingmode(opcode opc)
{
  return &AMTable[OPCodeTable[opc].am]; 
}

static inline int
inst_isvalid(opcode opc)
{
  return OPCodeTable[opc].op != INV;
}

static inline void
inst_load(void)
{
  m.ireg.dbb = mem_get(m.ireg.abr);
}

static inline void
inst_store(void)
{
  mem_put(m.ireg.abr, m.ireg.dbb);
}

static inline address
push_byte(address v)
{
  address addr;
  byte sp = machine_sp_get();

  ADDR_SET(addr, sp, STACK_PAGE);

  mem_put_notrace(addr, ADDR_LOW(v));

  sp--;
  machine_sp_set(sp);

  return addr;
}

static inline void
inst_push8(byte v) 
{
  address addr = push_byte(v);

  if (TraceState == TRACE_ON) {
    if (TraceType == TRACE_ACTION) {
      ACTION(ACTCNT).hdr.id  = T_MEM_WR;
      ACTION(ACTCNT).hdr.len = sizeof(byte);
      ACTION(ACTCNT).addr = addr;
      memcpy(ACTION(ACTCNT).val, &v, ACTION(ACTCNT).hdr.len);
      ACTCNT++;
    }
  }
}

static inline void
inst_push16(address v) 
{
  address addr;

  push_byte(ADDR_HIGH(v));
  addr = push_byte(ADDR_LOW(v));

  if (TraceState == TRACE_ON) {
    if (TraceType == TRACE_ACTION) {
      ACTION(ACTCNT).hdr.id  = T_MEM_WR;
      ACTION(ACTCNT).hdr.len = sizeof(address);
      ACTION(ACTCNT).addr = addr;
      memcpy(ACTION(ACTCNT).val, &v, ACTION(ACTCNT).hdr.len);
      ACTCNT++;
    }
  }
}

static inline void
inst_push24(byte v[3]) 
{
  address addr;

  push_byte(v[0]);
  push_byte(v[1]);
  addr = push_byte(v[2]);

  if (TraceState == TRACE_ON) {
    if (TraceType == TRACE_ACTION) {
      ACTION(ACTCNT).hdr.id  = T_MEM_WR;
      ACTION(ACTCNT).hdr.len = sizeof(byte)*3;
      ACTION(ACTCNT).addr = addr;
      memcpy(ACTION(ACTCNT).val, &v, ACTION(ACTCNT).hdr.len);
      ACTCNT++;
    }
  }
}

static inline byte
inst_pop()
{
  address addr;
  byte sp = machine_sp_get();

  sp++;
  machine_sp_set(sp);

  addr = 0;
  ADDR_SET_HIGH(addr, STACK_PAGE);
  ADDR_SET_LOW(addr, sp);

  return mem_get(addr);
}

extern uint64_t inst_loop(uint64_t count);
#endif
