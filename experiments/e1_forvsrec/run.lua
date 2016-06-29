--local trtotable = require '../../process/trtotable'
local dx = require '../../process/dataxforms.lua'
local sm = require '../../models/simpleModel'
require 'lfs'

math.randomseed( os.time() )

local sf = string.format
local ex = os.execute

--Clean up past runs.
ex("make clean")

--Generate some images.
for i=1,2 do
   local start = math.random(255)
--   ex(sf("echo make START=%d MAX=%d", start, start+10))
   ex("make START=" .. start .. " MAX=" .. start+10 )
end

--Generate traces, something fishy here. aborts 
ex("mkdir traces")
for file in lfs.dir("images") do
   ex("../../datasources/6502/6502 -t sv -o traces/" .. file .. ".trc images/" .. file .." /dev/null 2> tmp")
end

--Generate bmps
--broken no google code page
--Use stubs for now.

local rectr, recte = dx.gentrtetab("../../datasources/stub_data/exp1RecCountPngs/", 0.8, true)
local fortr, forte = dx.gentrtetab("../../datasources/stub_data/exp1ForCountPngs/", .8, true)

--Give recursive label 1
dx.tablelabel(rectr, 1)
dx.tablelabel(recte, 1)
--Give for loop label 2
dx.tablelabel(fortr, 2)
dx.tablelabel(forte, 2)

--print(#rectr)
--print(#recte)
--print(#fortr)
--print(#forte)


--Combine 4 tables into 2.
local trainSet = dx.cattables(rectr, fortr)
local testSet  = dx.cattables(recte, forte)

--print(#rectr + #fortr)
--print(#recte + #forte)

--print(#trainSet)
--print(#testSet) 

--Generate model
local mod = sm.buildModel(trainSet, 2)


sm.train(mod, trainSet)
sm.test(mod, testSet)



--[[
   print(file)
   local print(string.find(file, "%d+_"))
   print(string.find(file, "_%d+"))

end
--]]