#!/opt/local/bin/bash
declare -A AM
AM["---"]="&(AMTable[INVALIDAM])"
AM["A"]="&(AMTable[ACC])"      
AM["abs"]="&(AMTable[ABS])"      
AM["abs,X"]="&(AMTable[ABSX])"     
AM["abs,Y"]="&(AMTable[ABSY])"     
AM["#"]="&(AMTable[IMM])"      
AM["impl"]="&(AMTable[IMPL])"     
AM["ind"]="&(AMTable[IND])"      
AM["X,ind"]="&(AMTable[XIND])"     
AM["ind,Y"]="&(AMTable[INDY])"     
AM["rel"]="&(AMTable[REL])"      
AM["zpg"]="&(AMTable[ZP])"       
AM["zpg,X"]="&(AMTable[ZPX])"      
AM["zpg,Y"]="&(AMTable[ZPY])"      

typeset -i i
i=0
cat insttbl.txt | while read o a
do
    if [[ $o = '???' ]]; then o=INV; fi
    printf "{ 0x%02x,  ${AM[$a]}, 0, inst_$o, &(InstTable[$o]) },\n" $i; ((i++))
done