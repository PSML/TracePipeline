local stat = require '../../process/stat'
local csvtonn = require '../../process/csvtonn'
local models = require '../../models/models'
local trtotable = require '../../process/trtotable'
local tabletonn = require '../../process/tabletonn'
local lfs = require 'lfs'

if not (lfs.attributes("data", 'mode') == 'directory') then
   lfs.mkdir("data")
end

local inctr, incte = trtotable.gentrtetab("../../datasources/stub_data/exp3IncPngs/", 0.8, false)

--Use filenames to provide labels
inctr = trtotable.filenametolabel(inctr)
incte = trtotable.filenametolabel(incte)

--convert to tensor
--for k,v in pairs(inctr) do print(k) end
train = tabletonn.tabtonn(inctr)
test = tabletonn.tabtonn(incte)

local opt = models.get_opt()
print(opt)
--opt.model = "mlp"
opt.batchSize = 10
models.set_opt(opt)
models.run(train,test, 2)

