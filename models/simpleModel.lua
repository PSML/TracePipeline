--[[
Modified from:
https://github.com/torch/nn/blob/master/doc/training.md. 
--]]
require "nn"

local sm = {}

function sm.buildModel(trainTable, classes)
   -- make a multi-layer perceptron
   local mlp = nn.Sequential();  
   
   local inputs = trainTable[1][1]:size(1)
   --print(inputs)
   local HUs = 10; 

   mlp:add(nn.Linear(inputs , HUs))
   mlp:add(nn.Tanh()) --The crucial nonlinearity 
   mlp:add(nn.Linear(HUs, classes))
   mlp:add(nn.LogSoftMax())

   print(mlp);
   
   print("done genNet")
   --torch.save("the_mlp_net", mlp);
   return mlp
end

function sm.train(mlp, trainTable)
   
   print(trainTable.size)

   --Need to provide this function for training.
   function trainTable:size() return #trainTable end
   --This is the loss function, MSE.
   criterion = nn.ClassNLLCriterion();
   --This is the training technique, SGD.
   trainer = nn.StochasticGradient(mlp, criterion);
   --Default learning rate is 0.01
   --trainer.learningRate = 0.01
   --trainer.maxIteration = 100
   --Train, does 25 rounds by default
   
   --print(trainTable);
   trainer:train(trainTable);
   --trainer:train(3d_ten);
end

function sm.test(mlp, testTable)
   --Checks performance
   correct=0;
   for i=1,#testTable do
      --Forward propegate input.
      m = mlp:forward( testTable[i][1] ):exp();
      --   print(m);
      if m[1] > m[2]
      then
	 --    print("class1");
	 if testTable[i][2] == 1 then
	    correct = correct+1
	 end
      else
	 --      print("class2");
	 if testTable[i][2] == 2 then
	    correct = correct+1
	 end
      end
   end
   
   correct_percent = correct / #testTable;
   print("percent correct = "..correct_percent);
   
end


return sm
