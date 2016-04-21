--[[
Modified from:
https://github.com/torch/nn/blob/master/doc/training.md. 
--]]
processPath = "../process/";
--The datasets.
trainTable = torch.load( processPath.."outputTrainTable" );

require "nn"
-- make a multi-layer perceptron
mlp = nn.Sequential();  

--[[
I\'m not sure this is good form. This assumes all traces have the same dimension, which is guaranteed by process. It says multiply the length of the first and second dimension of the first component of the first table element. This is the row and col dimensions of the traces. One input node per pixel. Hopefully we find much smaller and more effective architectures in the future. 
--]]

inputs = (#trainTable[1][1])[1] * (#trainTable[1][1])[2]; 
classes = 2; HUs = 20; 

mlp:add(nn.Linear(inputs, HUs))
mlp:add(nn.Tanh()) --The crucial nonlinearity 
mlp:add(nn.Linear(HUs, classes))
mlp:add(nn.LogSoftMax())


torch.save("the_mlp_net", mlp);
