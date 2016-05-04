require 'nn';
processPath = "../../process/";
--The train dataset.
testTable  = torch.load( processPath.."outputTestTable"  );
--The trained net.
mlp = torch.load("trained_mlp_net");


--Checks performance
correct=0;
for i=1,#testTable do
   --Forward propegate input.
   m = mlp:forward( testTable[i][1] ):exp(); 
   --find index of greatest prob
   maxs, indices = torch.max(m, m:dim())
   
   print("predicted ");
   print(indices) 
   print(" actually " );
   print(testTable[i][2]);
   
   --When prediction is correct
   if indices == testTable[i][2] then
      correct = correct + 1;
   end

end

correct_percent = correct / #testTable;
print("percent correct = "..correct_percent);

--[[
x[1] =  0.5; x[2] =  0.5; print(mlp:forward(x))
x[1] =  0.5; x[2] = -0.5; print(mlp:forward(x))
x[1] = -0.5; x[2] =  0.5; print(mlp:forward(x))
x[1] = -0.5; x[2] = -0.5; print(mlp:forward(x))
--]]