--[[
This is copied from the torch nn documentation: https://github.com/torch/nn/blob/master/doc/training.md. I plan to model my first network on it. 
--]]


--[[
Generate 2d data. Pick random x and y values from the range (-1, 1).
If x_i and y_i have the same sign, output_i is -1, else 1. 
--]]

dataset={};
function dataset:size() return 100 end -- 100 examples, kludge
for i=1,dataset:size() do 
   -- normally distributed example in 2d
   local input = torch.randn(2);     
   local output = torch.Tensor(1);
   -- calculate label for XOR function
   if input[1]*input[2]>0 then     
      output[1] = -1;
   else
      output[1] = 1
   end
   dataset[i] = {input, output}
end


require "nn"
-- make a multi-layer perceptron
mlp = nn.Sequential();  
-- parameters
inputs = 2; outputs = 1; HUs = 20; 
mlp:add(nn.Linear(inputs, HUs))
mlp:add(nn.Tanh())
mlp:add(nn.Linear(HUs, outputs))

--This is the loss function, MSE.
criterion = nn.MSECriterion()  

--This is the training technique, SGD.
trainer = nn.StochasticGradient(mlp, criterion)
--This may be redundant.
print(trainer.learningRate)
trainer.learningRate = 0.01
--Train, does 25 rounds by default
trainer:train(dataset)

--Checks performance
x = torch.Tensor(2)
x[1] =  0.5; x[2] =  0.5; print(mlp:forward(x))
x[1] =  0.5; x[2] = -0.5; print(mlp:forward(x))
x[1] = -0.5; x[2] =  0.5; print(mlp:forward(x))
x[1] = -0.5; x[2] = -0.5; print(mlp:forward(x))