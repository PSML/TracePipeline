require 'nn';
--The net.
mlp = torch.load("the_mlp_net");
processPath = "../../processExp1/";
--The datasets.
trainTable = torch.load( processPath.."outputTrainTable" );

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


print(trainTable);

trainer:train(trainTable);
--trainer:train(3d_ten);

torch.save("trained_mlp_net", mlp)