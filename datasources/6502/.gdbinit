define sbreak
  set $pcaddr = &(m->vs.ms.reg.data.pc)
  watch (*((address *)$pcaddr) == $arg0 )
end
document sbreak
set a simulated break point on pc == $arg0
end

define sxbyte
  if $argc == 1
   set $sxbaddr = $arg0
   set $sxbcnt = 1
  end

  if $argc == 2
   set $sxbaddr = $arg0
   set $sxbcnt = $arg1
  end

  set $c = $sxbcnt
  set $d = 0

  while $c > 0
    set $lb = m->vs.ms.memory[$sxbaddr]
    set $c = $c - 1
    
    if $d == 0
      printf "0x%04x: 0x%02x (%c)", $sxbaddr, $lb, $lb
      set $d = $d + 1
    else
      printf " 0x%02x (%c)", $lb, $lb
      if $d == 3 
         printf "\n"
         set $d = 0
      else
         set $d = $d + 1
      end
    end
    set $sxbaddr = $sxbaddr + 1
  end
  printf "\n"
end
document sxbyte
print value of simulated memory at address arg0
end

define sxshort

  if $argc == 1
   set $sxsaddr = $arg0
   set $sxscnt = 1
  end

  if $argc == 2
   set $sxsaddr = $arg0
   set $sxscnt = $arg1
  end

  set $c = $sxscnt
  set $d = 0

  while $c > 0
    set $lb = m->vs.ms.memory[$sxsaddr]
    set $hb = m->vs.ms.memory[$sxsaddr+1]
    set $c = $c - 1
    
    if $d == 0
       printf "0x%04x: 0x%02x%02x", $sxsaddr, $hb, $lb
       set $d = $d + 1
    else
      printf " 0x%02x%02x", $hb, $lb
      if $d == 3 
         printf "\n"
         set $d = 0
      else
         set $d = $d + 1
      end
    end
    set $sxsaddr = $sxsaddr + 2
  end
  printf "\n"
end
document sxshort
  print address located in memory at address $arg0
end
 
define sopdecode
   set $opdesc = &OPCodeTable[$arg0]
   p *$opdesc
   p AMTable[$opdesc->am]
   p InstTable[$opdesc->ins]
end
document sopdecode
   print the details of opcode = $arg0	
end

define sdisassemble

  if $argc == 1
   set $sdaddr = $arg0
   set $sdcnt = 1
  end

  if $argc == 2
   set $sdaddr = $arg0
   set $sdcnt = $arg1
  end

  set $d = $sdcnt
  while $d > 0

    set $op = m->vs.ms.memory[$sdaddr]
    set $opdesc = &OPCodeTable[$op]
    set $am = &AMTable[$opdesc->am]
    set $bytes = $am->bytes
    set $fmt   = $am->prfmt
    set $ins   = &InstTable[$opdesc->ins]
 
    set $d = $d - 1

    if $bytes == 1
          printf "0x%04x:           : %s\n", $sdaddr, $ins->memonic 
    else 
      if $bytes == 2
          printf "0x%04x: 0x%02x      : %s 0x%02x   (%s)\n", $sdaddr, m->vs.ms.memory[$sdaddr+1], $ins->memonic, m->vs.ms.memory[$sdaddr+1], $fmt
      else     
        if $bytes == 3
          printf "0x%04x: 0x%02x 0x%02x : %s 0x%02x%02x (%s)\n", $sdaddr, m->vs.ms.memory[$sdaddr+1], m->vs.ms.memory[$sdaddr+2], $ins->memonic, m->vs.ms.memory[$sdaddr+2], m->vs.ms.memory[$sdaddr+1], $fmt 
        else 
	  printf "ERROR: invalid opcode? 0x%04x opcode=0x%02x\n", $sdaddr, $op
        end
      end
    end
    set $sdaddr = $sdaddr + $bytes

  end

end

document sdisassemble
  sdisassemble <addr> [count], sdissaemble invoked with no arguments will disassemble from the last address for the last count specified
end
  

define srst
  sxs 0xFFFC
end
document srst
  print the address of the installed reset interrupt service routine
end

define snmi
  sxs 0xFFFA
end
document snmi
  print the address of the installed non-maskable interrupt service routine
end

define sirq
  sxs 0xFFFE
end
document sirq
  print the address of the installed irq interrupt service routine
end

define sreg
  p /x m->vs.ms.reg
end
document sreg
  print 6502 registers
end

define ssp
  sxs 0x0000
end
document ssp
  print cc65 argument stackpointer (initialized to 0xFEFF in crt0.s)
end

define ssbyte
  set $lb = m->vs.ms.memory[0x0000]
  set $hb = m->vs.ms.memory[0x0001]
  set $ad = ($hb << 8) | $lb
  set $num = (0xfeff - $ad)
  sxb $ad $num
end
document ssb
  print cc65 argument stack as bytes
end

define ssshort
  set $lb = m->vs.ms.memory[0x0000]
  set $hb = m->vs.ms.memory[0x0001]
  set $ad = ($hb << 8) | $lb
  set $num = (0xfeff - $ad) / 2
  sxs $ad $num
end
document ssb
  print cc65 argument stack as shorts/addresses
end

define sbt
  set $sbtaddr = 0x01fe
  set $sbttaddr = ((0x01 << 8) | m->vs.ms.reg.data.sp)
 
  while $sbtaddr > $sbttaddr 
    set $lb = m->vs.ms.memory[$sbtaddr]
    set $hb = m->vs.ms.memory[$sbtaddr+1]
    set $sbtca = (($hb << 8) | $lb) - 2
    printf "(sp=0x%04x) ", $sbtaddr
    sd $sbtca 1
    set $sbtaddr = $sbtaddr - 2
  end

end
document sbt
  print attempt to produce a back trace from 6502 stack page this may not alway work as there maybe other values pushed on the stack other than JSR addresses
end
define sinfo 
  printf "pc=0x%04x ac=0x%02x x=0x%02x y=0x%02x sr=0x%02x sp=0x01%02x asp=0x%02x%02x srbits:\n",  m->vs.ms.reg.data.pc, m->vs.ms.reg.data.ac, m->vs.ms.reg.data.x, m->vs.ms.reg.data.y, m->vs.ms.reg.data.sr, m->vs.ms.reg.data.sp, m->vs.ms.memory[1], m->vs.ms.memory[0]
  p/t m->vs->ms.reg.data.sr
end
document sinfo
  pretty print register state
end

define strace
  b fetch
end
document strace
  set breakpoint for single stepping simulated instructions
end

define sstep
  c
  sinfo
  sd m->vs.ms.reg.data.pc
end
document sstep
  single step simulated instruction
end
   


  