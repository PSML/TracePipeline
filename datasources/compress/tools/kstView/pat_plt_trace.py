#!/usr/bin/python2.7
import pykst as kst
from PyQt4 import QtCore, QtNetwork, QtGui
from Tkinter import *
from numpy import *
import sys
import sip
sip.setapi('QString', 1)

if len(sys.argv) < 3:
    print("USAGE: " + sys.argv[0] + "[.pat file] [.seq file]")
    exit(0)

patfile = sys.argv[1]
seqfile =sys.argv[2]

class App: 

    def __init__(self, master):

        self.client=kst.Client()

        # Plot
        self.idx_vec = kst.DataVector(self.client,seqfile,"Column 1","","","","","","")
        self.fid_vec = kst.DataVector(self.client,seqfile,"Column 3","","","","","","")

        self.eq = kst.NewCurve(self.client,self.idx_vec,self.fid_vec,False,False,False,False,False,False,"blue","","",False,True,3,1,False,"","",False,"","",False,False,True,False,True,True,False)
        
        # Scroll window
        
        frame = Frame(master) 
        frame.pack()

        self.quit = Button(frame, text = "Quit", command = frame.quit) 
        self.quit.pack()

        self.select = Button(frame, text = "Select", command = self.get_selected)
        self.select.pack()

        self.select_patterns = Button(frame, text = "Select Patterns", command = self.get_selected_patterns)
        self.select_patterns.pack()

        self.clear = Button(frame, text = "Clear", command = self.clear_plot)
        self.clear.pack()
        
        self.selected = StringVar()
        self.selected.set("     ")
        self.selected_label = Label(frame, textvariable = self.selected, relief = RAISED, justify = LEFT, wraplength = 200)
        self.selected_label.pack()
        
        self.pattern = StringVar()
        self.pattern.set("Instruction Length Count")
        self.pattern_label = Label(frame, textvariable = self.pattern, relief = RAISED, justify = LEFT, wraplength = 200)
        self.pattern_label.pack()
        
        scrollbar = Scrollbar(root)
        scrollbar.pack(side=RIGHT, fill=Y)

        self.patternbox = Listbox(root, yscrollcommand = scrollbar.set)
        self.patternbox.pack()
        self.get_scroll_patterns(self.patternbox)

    def get_selected(self):
        linenum = self.patternbox.curselection()
        if linenum:
            index = linenum[0]
            point = self.line_list[index]
            point = point.split(" ")
            self.selected.set("inst: " + point[0])

        # Highlights selected pattern
        inst = int(point[0])
        length = int(point[1])
        self.highlight_point(inst, length)
        
    def get_selected_patterns(self):
        # Highlights the selected pattern and its repetitions in red
        linenum = self.patternbox.curselection()
        if linenum:
            index = linenum[0]
            point = self.line_list[index]
            point = point.split(" ")
            self.selected.set("inst: " + point[0])

        inst = int(point[0])
        length = int(point[1])
        #self.highlight_point(inst, length)

        # Highlight the repeats of that pattern
        # First find the occurrence of [inst, len, ...], save the patidx
        for i in range(len(self.lines)):
            if int(self.lines[i][0]) == inst:
                if int(self.lines[i][1]) == length:
                        patidx = self.lines[i][2]

        ids = []
        for j in range(len(self.lines)):
            id = int(self.lines[j][2])
            if id == int(patidx):
                #print "hi"
                self.highlight_point(int(self.lines[j][0]), length)
 
    def highlight_point(self, inst, length):
        # Highlights a given point in red

        pat = array(range(inst,inst+length))*1.0
        pat_idx_vec = kst.EditableVector(self.client)
        pat_idx_vec.setFromList(pat)

        vectors = kst.ExistingVector.getList(self.client)
        print vectors
        for v in vectors:
            if "V2" in v.getHandle():
                for i in range(inst,inst+length):
                    pat[i-inst] = v.value(i)

        pat_fid_vec = kst.EditableVector(self.client)
        pat_fid_vec.setFromList(pat)

        vectors = kst.ExistingVector.getList(self.client)
        self.selected_point = kst.NewCurve(self.client,pat_idx_vec,pat_fid_vec,False,False,False,False,False,False,"red",\
            "","",False,True,3,1,False,"","",False,"","",False,self.eq,False,False,True,True,False)
        
    def clear_plot(self):
        # Remakes the plot in blue
        self.original = kst.NewCurve(self.client, self.idx_vec, self.fid_vec,False,False,False,False,False,False,\
            "blue","", "", False, True, 3, 1, False, "", "", False, "", "", False, self.eq, False, False, True, True, False)
        
    def get_scroll_patterns(self, listbox):
        # Puts the starting points of the repeated patterns (from patfile) in the scroll window. Shows the instruction number, 
        # the length of the pattern, and the number of times that the pattern was repeated

        patfile_raw = open(patfile, 'r')
        patfile = patfile_raw.read()
        patfile_raw.close()
        patfile = patfile.replace("\n", " ")
        patfile = patfile.split(" ")
        patfile = patfile[4:]
        
        # make a list of tuples of each line (without the first column)
        self.lines = []
        patfile2 = open(patfile, 'r')
        lines_unedited = patfile2.readlines()
        patfile2.close()
        numlines = len(lines_unedited) - 1  
 
        for i in range(numlines):
            tup = [patfile[4 * i + 1], patfile[4 * i + 2], patfile[4 * i + 3]]
            self.lines.append(tup)
 
        unique_insts = [[self.lines[0][0], self.lines[0][1]]]
        
        dictionary = {}
        for n in range(len(self.lines)):
            m = int(self.lines[n][2])
            if dictionary.has_key(m) == False:
                dictionary[m] = [self.lines[n][0], self.lines[n][1]]
        
        patids = []
        for j in range(len(self.lines)):
            patids.append(self.lines[j][2])

        self.line_list = []
        for k in range(len(dictionary)):
            count = patids.count(str(k)) - 1
            line = dictionary[k][0] + " " + dictionary[k][1] + " " + str(count)
            self.line_list.append(line)
            listbox.insert(END, line)

root = Tk()
app = App(root)
root.mainloop()

