--Tool for unpacking csv's into {data, label, size()} tables w/ meta indexing 
--require 'csvigo'

local csvtonn = {}

math.randomseed(os.time())

function csvtonn.shuffleTensor(t)
   --This doesn't return the same storage
   --indexes into t using a random shuffled indices
   return t:index(1,torch.randperm(t:size(1)):long())
end

function csvtonn.splitTrainTest(t, p)
   --p is percent in train set.
   assert( p >= 0 and p <= 1, "P must be in range [0,1], was" .. p)
   --This only works if data is shuffled; do so by default.   
   t = csvtonn.shuffleTensor(t)

   local total_ex = t:size(1)
   local size_tr = math.floor(total_ex * p)
   local size_te = total_ex - size_tr
   
   local train = t:narrow(1, 1, size_tr)
   local test = t:narrow(1, size_tr+1, size_te) 

   return train, test
end

function csvtonn.splitDataLabels(t, feat_st, num_feat, lab_st, num_lab)
   --Assumes features contiguous, labels contiguous. 
   local new_t = {}
   new_t.data = t:narrow(2, feat_st, num_feat)
   --Only use labels if it has em.
   if lab_st ~= nil and num_lab ~= nil then
      new_t.labels = t:narrow(2, lab_st, num_lab)
   end
   return new_t
end

function csvtonn.prepForNN(t, num_classes)
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

return csvtonn


--[[
tr = csvigo.load({path = "/Users/tu/Documents/MLProject/datasources/hedge/numerai_training_data.csv", mode="raw"})

--to = csvigo.load({path = "/Users/tu/Documents/MLProject/datasources/hedge/numerai_tournament_data.csv", mode="raw"})

--kill header
table.remove(tr, 1)
--table.remove(to, 1)

--convert to tensor
tr_ten = torch.Tensor(tr)

mini_tr_ten = torch.Tensor(tr)
mini_tr_ten = mini_tr_ten:narrow(1, 1, 10000)

--to_ten = torch.Tensor(to)

--Implicit shuffle then split.
p = 0.85
train, test = splitTrainTest(tr_ten, p)
mini_train, mini_test = splitTrainTest(mini_tr_ten, p)

--separate into data and labels, this just comes from observing data 
start_features = 1
num_features = 21
start_labels = 22
num_labels = 1
num_classes = 2

--Separate single tensor into data and labels tensor.
train = splitDataLabels(train, start_features, num_features, start_labels, num_labels)
test = splitDataLabels(test, start_features, num_features, start_labels, num_labels)

mini_train = splitDataLabels(mini_train, start_features, num_features, start_labels, num_labels)
mini_test = splitDataLabels(mini_test, start_features, num_features, start_labels, num_labels)

--tournament = splitDataLabels(to_ten, start_features, num_features, nil, nil)
--add size and override index operation
--override index method to give {data, label}

train = prepForNN(train, num_classes)
test = prepForNN(test, num_classes)

mini_train = prepForNN(mini_train, num_classes)
mini_test = prepForNN(mini_test, num_classes)


--tournament = prepForNN(tournament, nil)
--]]
--[[
print('train')
print(train:size())
print(train.labels:sum())

print('test')
print(test:size())
print(test.labels:sum())

print(train:size() + test:size())

print('tournament')
print(tournament:size())
print(tournament)
--]]

