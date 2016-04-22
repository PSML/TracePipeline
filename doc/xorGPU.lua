--A modified version of the on the fly net https://github.com/torch/nn/blob/master/doc/training.md
require 'cunn'
mlp = nn.Sequential();  -- make a multi-layer perceptron
inputs = 2; outputs = 1; HUs = 20; -- parameters
mlp:add(nn.Linear(inputs, HUs))
mlp:add(nn.Tanh())
mlp:add(nn.Linear(HUs, outputs))

--Cudafy 
mlp = mlp:cuda();

for i = 1,2500 do
  -- random sample
   --Cudafy
   local input= torch.randn(2):cuda();     -- normally distributed example in 2d
   local output= torch.Tensor(1):cuda();
   if input[1]*input[2] > 0 then  -- calculate label for XOR function
      output[1] = -1
   else
      output[1] = 1
   end
   --Cudafy
   criterion = nn.MSECriterion():cuda();
  -- feed it to the neural network and the criterion
   criterion:forward(mlp:forward(input), output)

  -- train over this example in 3 steps
   -- (1) zero the accumulation of the gradients
   mlp:zeroGradParameters()
   -- (2) accumulate gradients
   --Watch out, overloading "output" here.
   mlp:backward(input, criterion:backward(mlp.output, output))
   -- (3) update parameters with a 0.01 learning rate
   mlp:updateParameters(0.01)
end

--Cudafy
x = torch.Tensor(2):cuda();
x[1] =  0.5; x[2] =  0.5; print(mlp:forward(x))
x[1] =  0.5; x[2] = -0.5; print(mlp:forward(x))
x[1] = -0.5; x[2] =  0.5; print(mlp:forward(x))
x[1] = -0.5; x[2] = -0.5; print(mlp:forward(x))