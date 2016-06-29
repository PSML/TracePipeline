#!/opt/local/bin/bash

grep  '0x..,' optbl.h | while read b0 opc am ins rest
do
  opc=${opc%%,*}
  am=${am%%,*}
  ins=${ins%%,*}
  echo "case ${opc}:"
  echo "  /* $opc $am $ins */"
  echo "  inst_am_${am,,*}();"
  echo "  pc += AM_${am}_BYTES;"
  echo "  machine_pc_set(pc);"
  echo "  rc = inst_${ins}();"
  echo "  break;"
done