import sys

if len(sys.argv) == 2:
  f = open(sys.argv[1], 'r')
  o = open(sys.argv[1] + '.gv', 'w')
  #o = open(sys.argv[1] + '.' + sys.argv[2] + '.gv', 'w')
  #p = int(sys.argv[2])
  p = 0

  o.write('digraph G {\n')

  count = []
  firstline = True
  for line in f:
    if 'dotted' in line:
      o.write(line)
      continue
    r = line.split(' -> ')
    if firstline:
      count = [0 for x in range(int(r[0]) + 1)]
      firstline = False
    if 'label=1' in line.split(';')[1]:
      if p == 0:
        o.write(line)
        #tmp = line.split(';')[0].split('label=')[1].strip(']')
        #print tmp + ':c=1:l=' + tmp;
      c = 1
    else:
      n = r[1].split(' ')
      c = count[int(n[0])]
      if c > p:
        o.write(line.rstrip('\n') + ' ' + n[0] + ' [label=' + str(c) + '];\n')
        #tmp = line.split(';')[0].split('label=')[1].strip(']')
        #print str((c * int(tmp))) + ':c=' + str(c) + ':l=' + tmp
    #print line, count
    count[int(r[0])] += c
    #print count
  o.write('}\n')
else:
  print 'USAGE: python ' + sys.argv[0] + ' [reverse sorted rule file]'
