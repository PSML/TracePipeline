local dx = {} 

math.randomseed(os.time())

function dx.shuffleTensor(t)
   --This doesn't return the same storage
   --indexes into t using a random shuffled indices
   return t:index(1,torch.randperm(t:size(1)):long())
end

function dx.splitTrainTest(t, p)
   --p is percent in train set.
   assert( p >= 0 and p <= 1, "P must be in range [0,1], was" .. p)
   --This only works if data is shuffled; do so by default.
   t = dx.shuffleTensor(t)

   local total_ex = t:size(1)
   local size_tr = math.floor(total_ex * p)
   local size_te = total_ex - size_tr

   local train = t:narrow(1, 1, size_tr)
   local test = t:narrow(1, size_tr+1, size_te)

   return train, test
end


function dx.splitDataLabels(t, feat_st, num_feat, lab_st, num_lab)
   --Assumes features contiguous, labels contiguous.
   local new_t = {}
   new_t.data = t:narrow(2, feat_st, num_feat)
   --Only use labels if it has em.
   if lab_st ~= nil and num_lab ~= nil then
      new_t.labels = t:narrow(2, lab_st, num_lab)
   end
   return new_t
end


function dx.prepForNN(t, num_classes)
   --class is a userdata not number
   if num_classes ~= nil then
      local labelvector = torch.zeros(num_classes)
   local mt = {
      __index = function(self, index)
                   local input = self.data[index]
                   local class = self.labels[index]:squeeze() -- this bug took me an hour
                   local label = labelvector:zero()
                   label[class+1] = 1
                   return {input, label}
                end
   }

   setmetatable(t,mt)
end
t.size = function() return t.data:size(1) end
   return t
end


function dx.tabtonn(table)
   --randomize data, this matters a lot.
   --total hack, do it multiple times b/c birthday paradox
   for j = 1, 5 do
      for i = 1, #table do -- backwards
         local r = math.random(i) -- select a random number between 1 and i
         table[i], table[r] = table[r], table[i] -- swap the randomly selected item to position i
      end
   end

   --Turn {{d1,l1}, {d2,l2} ...} into {data:{1: d1, 2: d2 ...}, labels:{1: l1, 2:l2 ...}}
   local data = {}
   local labels = {}
   for i=1,#table do
      data[i]  = table[i][1]
      labels[i] = table[i][2]
   end
   local new_table = {}
   new_table.data = data
   new_table.labels = labels

   --give a size field
   new_table.size = function() return #new_table.labels end

   --override index method to give {data, label}
   local labelvector = torch.zeros(9)
   local mt = {__index = function(self, index)
                            local class = self.labels[index]
                            local label = labelvector:zero()
                            label[class] = 1
                            return {self.data[index], label}
                         end
   }
   setmetatable(new_table, mt)
   return new_table
end

function dx.addsize(t)
   t.size = function() return #new_table.labels end
   return t
end

require 'paths';
require 'image';
torch.seed();

function dx.gentrtetab(dirpath, p, flatten)
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

function dx.tablelabel(t, l)
   for k, v in pairs(t) do
      v[2] = l
   end
end

function dx.cattables(t1,t2)
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


function dx.filenametolabel(t)
   for i=1,#t do
      t[i][2] = tonumber(string.sub(t[i][2], 1, 1))
   end
   return t
end

return dx