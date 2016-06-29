import sys

rules = dict()
lhash = dict()

def generate_graph(rlist, idx):
  for token in rlist:
    if token.find("&") != -1:
      idx += 1
    else:
      l = rules[token][1]
      c = 1.0*rules[token][2]/len(lhash[l])
      o.write('newline pts ' + str(idx) + ' ' + str(l) +
              ' ' + str(idx+l-1) + ' ' + str(l) +
              ' color ' + str(c) + ' 0 0\n')
      o.write('newline pts ' + str(idx) + ' 0 ' +
              str(idx) + ' ' + str(l) +
              ' color ' + str(c) + ' 0 0\n')
      o.write('newline pts ' + str(idx+l-1) + ' 0 ' +
              str(idx+l-1) + ' ' + str(l) +
              ' color ' + str(c) + ' 0 0\n')
      generate_graph(rules[token][0], idx)
      idx += l

if len(sys.argv) == 2:
  f = open(sys.argv[1], 'r')
  o = open(sys.argv[1] + '.jgr', 'w')

  o.write('newgraph\n')

  for line in f:
    r = line.split(' -> ')
    s = r[1].split('\t')
    if len(s) == 1:
      start = r[1].rstrip(' \n').split(' ')
      #print r[0], start
    else:
      l = len(s[1].split("&"))-1
      if l in lhash:
        lhash[l].append(r[0])
      else:
        lhash[l] = [r[0]]
      rules[r[0]] = (s[0].rstrip(' ').split(' '), l, len(lhash[l]))
      #print r[0], rules[r[0]]

  generate_graph(start, 0)
else:
  print 'USAGE: python ' + sys.argv[0] + ' [rule file]'
