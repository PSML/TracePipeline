require 'cunn'
--The net.
mlp = torch.load("the_mlp_net_GPU");
processPath = "../../process/";
--The datasets.
trainTable = torch.load( processPath.."outputTrainTable" );


function trainTable:size() return #trainTable end

--trainTable is a ... table. Convert elements to tensor.

for i = 1,#trainTable do
   input = trainTable[i][1]:cuda();
   local target = torch.Tensor(1):cuda();
   target[1] = trainTable[i][2];

   criterion = nn.ClassNLLCriterion():cuda();   
   criterion:forward(mlp:forward(input), target)
   
   mlp:zeroGradParameters()
   
   mlp:backward(input, criterion:backward(mlp.output, target))
   mlp:updateParameters(0.01)

end


--This is the training technique, SGD.
--trainer = nn.StochasticGradient(mlp, criterion);
--Default learning rate is 0.01
--trainer.learningRate = 0.01
--trainer.maxIteration = 100

--Train, does 25 rounds by default
--trainer:train(trainTable);

torch.save("trained_mlp_net_GPU", mlp)
