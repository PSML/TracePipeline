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

--Generate traces, something fishy here.
ex("mkdir traces")
for file in lfs.dir("images") do
   ex("../../datasources/6502/6502 -t sv -o traces/" .. file .. ".trc images/" .. file .." /dev/null 2> tmp")
end

--Generate bmps
--broken no google code page

--Generate torch tensors from bmps

--Run through model 

--[[
   print(file)
   local print(string.find(file, "%d+_"))
   print(string.find(file, "_%d+"))

end
--]]