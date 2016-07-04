local models = require '../../models/models'
local dx = require '../../process/dataxforms'
local csvigo = require 'csvigo'

--Prep torch nn compatible table of tensors from csv.
tr_ten = dx.csvtotensor("data/train_baseline.csv", true)
te_ten = dx.csvtotensor("data/test_baseline.csv", true)

tr_ten = dx.splitDataLabels(tr_ten, 2, 110, 1, 1)
te_ten = dx.splitDataLabels(te_ten, 2, 110, 1, 1)

local num_classes = 2

tr_ten = dx.prepForNN(tr_ten, num_classes)
te_ten = dx.prepForNN(te_ten, num_classes)

local opt = models.get_opt()
print(opt)
opt.model = "mlp"
opt.optimizaion = "LBFGS"
opt.momentum = 0.1
opt.coefL2 = 0.00001
models.set_opt(opt)
opt.batchsize=101
models.run(tr_ten,te_ten, 1)

