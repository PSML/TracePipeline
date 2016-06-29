import sip
sip.setapi('QString', 1)
import pykst as kst
import sys
import random
from PyQt4 import QtCore, QtNetwork, QtGui
from Tkinter import *

print 'Argument list: ', str(sys.argv)
filename = sys.argv[1]

# Ordering of registers: pc, ac, y, x, sp, sr

class App: 

    def __init__(self, master):

        self.client=kst.Client()
        
        frame = Frame(master) 
        frame.pack()
        
        self.var = StringVar()
        self.var.set("      ")
        self.label = Label(frame, textvariable = self.var, relief = RAISED, justify = LEFT, wraplength = 200)
        self.label.pack()

        self.var2 = StringVar()
        self.var2.set("      ")
        self.label2 = Label(frame, textvariable = self.var2, relief = RAISED, justify = LEFT, wraplength = 200) 
        self.label2.pack()

        self.pc = StringVar()
        self.pc.set("      ")
        self.pc_label = Label(frame, textvariable = self.pc, relief = RAISED, justify = LEFT, wraplength = 200)
        self.pc_label.pack()

        self.ac = StringVar()
        self.ac.set("      ")
        self.ac_label = Label(frame, textvariable = self.ac, relief = RAISED, justify = LEFT, wraplength = 200)
        self.ac_label.pack()

        self.y = StringVar()
        self.y.set("      ")
        self.y_label = Label(frame, textvariable = self.y, relief = RAISED, justify = LEFT, wraplength = 200)
        self.y_label.pack()

        self.x = StringVar()
        self.x.set("      ")
        self.x_label = Label(frame, textvariable = self.x, relief = RAISED, justify = LEFT, wraplength = 200)
        self.x_label.pack()

        self.sp = StringVar()
        self.sp.set("      ")
        self.sp_label = Label(frame, textvariable = self.sp, relief = RAISED,justify = LEFT, wraplength = 200)
        self.sp_label.pack()

        self.sr = StringVar()
        self.sr.set("      ")
        self.sr_label = Label(frame, textvariable = self.sr, relief = RAISED, justify = LEFT, wraplength = 200)
        self.sr_label.pack()

        self.memstr = StringVar()
        self.memstr.set("      ")
        self.memory_label = Label(frame, textvariable = self.memstr, relief = RAISED, justify = LEFT, wraplength = 200)
        self.memory_label.pack()

        #self.chosen = StringVar()
        #self.chosen.set("    ")
        #self.chosen_label = Label(frame, textvariable = self.chosen, relief = RAISED)
        #self.chosen_label.pack()

        # self.select = Button(frame, text = "Select", command = self.make_selection())
        # self.select.pack()
        
        self.quit  = Button(frame, text = "Quit", command = frame.quit) 
        self.quit.pack(side = LEFT) 
        
        self.inspect = Button(frame, text = "Inspect", command = self.get_coords)
        self.inspect.pack(side = LEFT)
        
    def get_coords(self):
        # Insert function to get (x, y) coordinates here. Arbitrary coordinates are used here
        # (self) -> None

        self.coords = str(kst.PSMLGetSelected(self.client)).split(' ')
        self.coords[0] =  int(float(self.coords[0]))
        self.coords[1] = int(float(self.coords[1]))
        self.coords = self.coords[0], self.coords[1]
        self.var.set("(time, fid) : " + str(self.coords))
        self.fid = self.coords[1]
        self.xor = self.get_xor(self.fid)
        self.var2.set(str(self.xor))
        self.set_registers(self.xor)

    def get_xor(self, fid):
        # Reads from the hist file, looks up the given fid and returns the corresponding XOR
        # (self, int) -> (str)
        file = open(filename, 'r')
        lines =  file.readlines()
        file.close()
        line = lines[fid]
        line = line.rstrip("\n")
        line = line.split(" ")
        xor = line[len(line) - 1]
        return xor

    def set_registers(self, xor):
        # Separates the xor into the registers
        # (str) -> None
        line = xor.split("||") # header || registers || memory (add|ress|value)
        # Display the registers
        if len(line) >= 2:
            regs = line[1]
            regs = regs.split('|')
            regs[:] = [x for x in regs if x != '']
            num_regs = len(regs) - 1
            reg_names = ['pc', 'ac', 'y', 'x', 'sp', 'sr']

            if num_regs == 6:
                self.pc.set("pc : " + regs[0] + " " + regs[1])
                self.ac.set("ac : " + regs[2])
                self.y.set("y : " + regs[3])
                self.x.set("x : " + regs[4])
                self.sp.set("sp : " + regs[5])
                self.sr.set("sr : " + regs[6])
            
            if num_regs == 5: 
                self.pc.set("pc : " + regs[0] + " " + regs[1])
                self.ac.set("ac : " + regs[2])
                self.y.set("y : " + regs[3])
                self.x.set("x : " + regs[4])
                self.sp.set("sp : " + regs[5])
                self.sr.set(" ")
        
            if num_regs == 4: 
                self.pc.set("pc : " + regs[0] + " " + regs[1])
                self.ac.set("ac : " + regs[2])
                self.y.set("y : " + regs[3])
                self.x.set("x : " + regs[4])
                self.sp.set(" ")
                self.sr.set(" ")   

            if num_regs == 3: 
                self.pc.set("pc : " + regs[0] + " " + regs[1])
                self.ac.set("ac : " + regs[2])
                self.y.set("y : " + regs[3])
                self.x.set(" ")
                self.sp.set(" ")
                self.sr.set(" ")

            if num_regs == 2: 
                self.pc.set("pc : " + regs[0] + " " + regs[1])
                self.ac.set("ac : " + regs[2])
                self.y.set(" ")
                self.x.set(" ")
                self.sp.set(" ")
                self.sr.set(" ")

            if num_regs == 1: 
                self.pc.set("pc : " + regs[0] + " " + regs[1])
                self.ac.set(" ")
                self.y.set(" ")
                self.x.set(" ")
                self.sp.set(" ")
                self.sr.set(" ")

        # Display the memory 
        if len(line) == 3:
            mem = line[2]
            mem = mem.split('|')
            mem[:] = [z for z in mem if z != '']
            num_mems = len(mem) / 3
            str1 = ''
            for j in range(0, num_mems):
                str1 = str1 + "Mem " + str(j) + ": addr " +  mem[3*j] + " " + mem[3*j + 1] + " val "+ mem[3*j + 2] + " " 
                self.memstr.set(str1) 
        else:
            self.memstr.set(" ")

root = Tk()
app = App(root)
root.mainloop()
        
        










