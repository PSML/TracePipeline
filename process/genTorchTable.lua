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
recPath = '../data/recCountpngs/';
forPath = '../data/forCountpngs/';
recTmp   = {}; --Holds all records of rec.
forTmp   = {}; --Holds all records of for.

require 'paths';
require 'image';

--Recursive count traces.
for f in paths.iterfiles( recPath ) do
   --Image loads in as 3d tensor. This selects out the 2 useful dimensions.
   --Then removes the final 8 elements and truncates it to 500 elements. 
   recT  = image.load(recPath..f):select(1,1):sub( 1,500 , 1,56 );
   --Then flattens image to 1d.
   recTmp[#recTmp+1] = recT:resize( (#recT)[1] * (#recT)[2] );
end

--For loop count traces.
for f in paths.iterfiles( forPath ) do
   forT = image.load(forPath..f):select(1,1):sub( 1,500 , 1,56 );
   forTmp[#forTmp+1] = forT:resize( (#forT)[1] * (#forT)[2] );
end

--The training set.
outputTrain = {};
--The testing set.
outputTest  = {};

--Fill out Training and testing sets with rec data.
for i = 1,#recTmp do 
   if torch.uniform() < 0.8 
   then
      outputTrain[ #outputTrain+1] = { recTmp[i] , 1 }; --Provide trace and label, 1
   else
      outputTest[ #outputTest+1] = { recTmp[i] , 1  };
   end
end

--Fill out Training and testing sets with for data.
for i = 1,#forTmp do 
   if torch.uniform() < 0.8 
   then
      outputTrain[ #outputTrain+1] = { forTmp[i] , 2 }; --Provide trace and label, 1
   else
      outputTest[ #outputTest+1] = { forTmp[i] , 2  };
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

