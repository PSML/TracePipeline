local models = require '../../models/models'
local dx = require '../../process/dataxforms'
local lfs = require 'lfs'

if not (lfs.attributes("data", 'mode') == 'directory') then
   lfs.mkdir("data")
end

local inctr, incte = dx.gentrtetab("../../datasources/stub_data/exp3IncPngs/", 0.8, false)

--Use filenames to provide labels
inctr = dx.filenametolabel(inctr)
incte = dx.filenametolabel(incte)

--convert to tensor
--for k,v in pairs(inctr) do print(k) end
train = dx.tabtonn(inctr)
test = dx.tabtonn(incte)

local opt = models.get_opt()
print(opt)
--opt.model = "mlp"
opt.batchSize = 10
models.set_opt(opt)
models.run(train,test, 2)

