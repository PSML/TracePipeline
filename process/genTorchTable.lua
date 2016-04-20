--[[
Here we generate 1 table from the traces. We assign 1 as the recursive label, 2 for the for loop traces. We won\'t worry about the order of the examples because we will make sure SGD is shuffleing them. We will partition the data into training and testing sets.
Note1: The traces are different lengths, we will truncate the longer to the length of the shorter.
Note2: There are only 7 bytes of register state. You can see this in the printout when you run:
$ ./6502 -v
Listing state vector components:
pc:0,2
ac:2,1
 y:3,1
 x:4,1
sp:5,1
sr:6,1
memory:8,65536
For this reason we will truncate the last 8 cols. 
--]]

