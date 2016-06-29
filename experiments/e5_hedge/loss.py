from subprocess import call
#from text import split

my_t = "th runNets.lua -m mlp -t 7 -b 200 -f -r 0.005"

#Run baseline
for i in range(10):
    fileName="data/errf100plus100xabsdiff{}.txt"
    f = open(fileName.format(i), "w")
    call(my_t.split(), stderr=f)
    f.close()

