--[[
Modified from:
https://github.com/torch/nn/blob/master/doc/training.md. 
--]]
processPath = "../../processExp3/";
--The datasets.
trainTable = torch.load( processPath.."outputTrainTable" );

require "nn"
-- make a multi-layer perceptron
mlp = nn.Sequential();  

--[[
I\'m not sure this is good form. This assumes all traces have the same dimension, which is guaranteed by process. It says multiply the length of the first and second dimension of the first component of the first table element. This is the row and col dimensions of the traces. One input node per pixel. Hopefully we find much smaller and more effective architectures in the future. 
--]]



inputs = 22120  --59584 --Fix this magic number
--print(inputs)
classes = 9; HUs = 10; HU2s=10;

mlp:add(nn.Linear(inputs , HUs))
mlp:add(nn.Tanh()) --The crucial nonlinearity 
mlp:add(nn.Linear(HUs, HU2s))
mlp:add(nn.Tanh()) --The crucial nonlinearity 
mlp:add(nn.Linear(HU2s, classes))
mlp:add(nn.LogSoftMax())

print(mlp);

torch.save("the_mlp_net", mlp);
