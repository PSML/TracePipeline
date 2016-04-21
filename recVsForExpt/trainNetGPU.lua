require 'nn';
require 'cutorch';
--The net.
mlp = torch.load("the_mlp_net");
processPath = "../process/";
--The datasets.
trainTable = torch.load( processPath.."outputTrainTable" );

mlp_c = mlp:cuda();
--trainTable_c = trainTable:cuda()  --This fails because trainTable is a table, not a tensor. 
--[[
input = torch.Tensor(1,2800,1);
for i= 1,#trainTable do
   input[i][i][1] = trainTable[i][1]
   input[i][i][2] = trainTable[i][2]
end
--]]


--Need to provide this function for training. 
function trainTable:size() return #trainTable end

--This is the loss function, MSE.
criterion = nn.ClassNLLCriterion():cuda();
--This is the training technique, SGD.
trainer = nn.StochasticGradient(mlp_c, criterion);
--Default learning rate is 0.01
--trainer.learningRate = 0.01
--trainer.maxIteration = 100

--Train, does 25 rounds by default
trainer:train(trainTable_c);

torch.save("trained_mlp_net_c", mlp_c)