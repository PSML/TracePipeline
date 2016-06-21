--[[
Modified from:
https://github.com/torch/nn/blob/master/doc/training.md. 
--]]
processPath = "../../processExp1/";
--The datasets.
trainTable = torch.load( processPath.."outputTrainTable" );

require "nn"
-- make a multi-layer perceptron
mlp = nn.Sequential();  

inputs = 28000 --Fix this magic number
--print(inputs)
classes = 2; HUs = 10; 

mlp:add(nn.Linear(inputs , HUs))
mlp:add(nn.Tanh()) --The crucial nonlinearity 
mlp:add(nn.Linear(HUs, classes))
mlp:add(nn.LogSoftMax())

print(mlp);

torch.save("the_mlp_net", mlp);
