local tabletonn = {}

function tabletonn.tabtonn(table)
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

function tabletonn.addsize(t)
   t.size = function() return #new_table.labels end
   return t
end

return tabletonn