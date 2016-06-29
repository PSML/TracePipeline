import sys

def print_digraph(num, rule):
  o.write('  ' + num + ' [shape=polygon, sides=5];\n')
  for r in rule:
    o.write('  ' + num + ' -> ' + r.replace('&','-') + ';\n')

if len(sys.argv) == 2:
  f = open(sys.argv[1], 'r')
  o = open(sys.argv[1] + '.gv', 'w')

  o.write('digraph G {\n')

  for line in f:
    r = line.split(' -> ')
    s = r[1].split('\t')
    if len(s) == 1:
      print_digraph(r[0], r[1].rstrip(' \n').split(' '))
      #print r[0], r[1].rstrip(' \n').split(' ')
    else:
      l = len(s[1].split("&"))-1
      print_digraph(r[0], s[0].rstrip(' ').split(' '))
      #print r[0], s[0].rstrip(' ').split(' ')

  o.write('}\n')
else:
  print 'USAGE: python ' + sys.argv[0] + ' [rule file]'
