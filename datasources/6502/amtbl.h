#ifndef __AMTBL_H__
#define __AMTBL_H__

#define AM_INVALIDAM_BYTES 0
#define AM_ACC_BYTES  1
#define AM_ABS_BYTES  3
#define AM_ABSX_BYTES 3
#define AM_ABSY_BYTES 3
#define AM_IMM_BYTES  2
#define AM_IMPL_BYTES 1
#define AM_IND_BYTES  3
#define AM_XIND_BYTES 2
#define AM_INDY_BYTES 2
#define AM_REL_BYTES  2
#define AM_ZP_BYTES   2
#define AM_ZPX_BYTES  2
#define AM_ZPY_BYTES  2


struct AddressingModeDesc AMTable[] = {
  { INVALIDAM,  AM_INVALIDAM_BYTES, "INVALID ADDRESSING MODE", "" },
  { ACC,        AM_ACC_BYTES, "A: Accumulator A", "" },
  { ABS,        AM_ABS_BYTES, "abs: Absolute a", "$HHLL" },
  { ABSX,       AM_ABSX_BYTES, "abs,X: Absolute Indexed with X a,x", "$HHLL,X" },
  { ABSY,       AM_ABSY_BYTES, "abs,Y: Absolute Indexed with Y a,y", "$HHLL,Y" },
  { IMM,        AM_IMM_BYTES, "#: Immediate #", "#$BB" },
  { IMPL,       AM_IMPL_BYTES, "impl: Implied i", "" },
  { IND,        AM_IND_BYTES, "ind: Absolute Indirect (a)", "$($HHLL)" },
  { XIND,       AM_XIND_BYTES, "X,ind: Zero Page Indexed Indirect (zp,x)", "($BB,X)" },
  { INDY,       AM_INDY_BYTES, "ind,Y: Zero Page Indirect Indexed with Y (zp),y", "($LL),Y" },
  { REL,        AM_REL_BYTES, "rel: Program Counter Relative r", "$BB" },
  { ZP,         AM_ZP_BYTES, "zpg: Zero Page zp", "$LL" },
  { ZPX,        AM_ZPX_BYTES, "zpg,X: Zero Page Index with X", "$LL,X" },
  { ZPY,        AM_ZPY_BYTES, "zpg,Y: Zero Page Index with Y", "$LL,Y" }
};

#endif
