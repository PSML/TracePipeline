--[[
Here we generate 1 table from the traces. We assign 1 as the recursive label, 2 for the for loop 
traces. We won\'t worry about the order of the examples because we will make sure SGD is 
shuffling them. We will partition the data into training and testing sets.
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
To Do: Clean this the hell up, it\'s embarrassing.
--]]
require 'paths';
require 'image';
torch.seed();

local trtotable = {}

function trtotable.gentrtetab(dirpath, p, flatten)
   --todo deal with max length
   --The training set.
   local outputTrain = {};
   --The testing set.
   local outputTest  = {};

   --   local trcTmp   = {}; --Holds all records of rec.
   --local forTmp   = {}; --Holds all records of for.
   --Recursive count traces.
   for f in paths.iterfiles( dirpath ) do
      --Image loads in as 3d tensor. This selects out the 2 useful dimensions.
      --Then removes the final 8 elements and truncates it to 500 elements. 
      local img = image.load(dirpath..f)
      --cropping should be own fn.
      local recT  = img:select(1,1):sub( 1, math.min(500, img:size(2))  , 1,56 );
      --Then flattens image to 1d.
      if flatten then
	 recT = recT:resize( (#recT)[1] * (#recT)[2] );
      end
      --Assign to train or test table.
      if torch.uniform() < 0.8 
      then
	 outputTrain[#outputTrain+1] = { recT , f }; --Provide trace and label, 1
      else
	 outputTest[#outputTest+1] = { recT , f  };
      end
   end

--   function outputTrain:size() return #outputTrain end
--   function outputTest:size()  return #outputTest  end
   return outputTrain, outputTest
end

function trtotable.tablelabel(t, l)
   for k, v in pairs(t) do
      v[2] = l
   end
end

function trtotable.cattables(t1,t2)
   local tmp = {}
   --Deep copy t1
   for i=1,#t1 do
      tmp[i] = t1[i]
   end

   for i=1,#t2 do
      tmp[#tmp+1] = t2[i]
   end
   tmp.size = #tmp
   return tmp
end

function trtotable.filenametolabel(t)
   for i=1,#t do
      t[i][2] = tonumber(string.sub(t[i][2], 1, 1))
   end
   return t
end

return trtotable