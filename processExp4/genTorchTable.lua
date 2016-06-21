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
To Do: Pull out magic numbers.
--]]

torch.seed();
dataPath = '../data/exp3IncPngs/';
dataTmp   = {}; --Holds all records of rec.

require 'paths';
require 'image';

labels = {}
dataTmp = {}
for f in paths.iterfiles( dataPath ) do
   --Image loads in as 3d tensor. This selects out the 2 useful dimensions.
   --Then removes the final 8 elements and truncates it to 500 elements.    
   dataTmp[#dataTmp+1] = {image.load(dataPath..f):select(1,1):sub( 1,395 , 1,56 ),
      string.byte( string.sub(f, 1, 1)  )-48  };
   
end

--The training set.
outputTrain = {};
--The testing set.
outputTest  = {};

for i = 1,#dataTmp do 
   --print("associating file: " .. f .. "  with inc: " .. labels[aa]);
   if torch.uniform() < .8 --This is forcing it to go to 1 table
   then
      outputTrain[ #outputTrain+1] =  dataTmp[i] ; --Provide trace and label, 1
   else
      outputTest[ #outputTest+1] = dataTmp[i] ;
   end
end

function outputTrain:size() return #outputTrain end
function outputTest:size()  return #outputTest  end

print("output Train:");
print(#outputTrain);
--print( outputTrain  );
print("output Test:");
print(#outputTest);
--print( outputTest  );

torch.save("outputTrainTable", outputTrain);
torch.save("outputTestTable", outputTest);

