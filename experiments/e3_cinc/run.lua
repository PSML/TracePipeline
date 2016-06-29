local dx = require '../../process/dataxforms'
local lfs = require 'lfs'
local sm = require '../../models/simpleModel'

if not (lfs.attributes("data", 'mode') == 'directory') then
   lfs.mkdir("data")
end

local inctr, incte = dx.gentrtetab("../../datasources/stub_data/exp3IncPngs/", 0.8, true)

local num_classes = 9
--Use filenames to provide labels
inctr = dx.filenametolabel(inctr)
incte = dx.filenametolabel(incte)

--todo incorporate this correctly
inctr = dx.addsize(inctr)
incte = dx.addsize(incte)


local mod = sm.buildModel(inctr, num_classes)
sm.train(mod, inctr)
sm.test(mod, incte)

