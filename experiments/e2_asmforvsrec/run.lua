local stat = require '../../process/stat'
local csvtonn = require '../../process/csvtonn'
local models = require '../../models/models'
local trtotable = require '../../process/trtotable'
local tabletonn = require '../../process/tabletonn'
local lfs = require 'lfs'
local sm = require '../../models/simpleModel'
if not (lfs.attributes("data", 'mode') == 'directory') then
   lfs.mkdir("data")
end

local inctr, incte = trtotable.gentrtetab("../../datasources/stub_data/exp2IncPngs/", 0.8, true)

local num_classes = 9

--Use filenames to provide labels
inctr = trtotable.filenametolabel(inctr)
incte = trtotable.filenametolabel(incte)

--todo incorporate this correctly
inctr = tabletonn.addsize(inctr)
incte = tabletonn.addsize(incte)


local mod = sm.buildModel(inctr, num_classes)
sm.train(mod, inctr)
sm.test(mod, incte)
