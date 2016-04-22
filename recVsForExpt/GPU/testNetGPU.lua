require 'cunn';
processPath = "../../process/";
--The train dataset.
testTable  = torch.load( processPath.."outputTestTable"  );
--The trained net.
mlp = torch.load("trained_mlp_net_GPU");


--Checks performance
correct=0;
for i=1,#testTable do
   --Forward propegate input.
   m = mlp:forward( testTable[i][1]:cuda() ):exp();
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
