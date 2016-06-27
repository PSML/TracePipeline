local stat = require '../../process/stat'
local csvtonn = require '../../process/csvtonn'
local models = require '../../models/models'

local lfs = require 'lfs'
local csvigo = require 'csvigo'

if not (lfs.attributes("data", 'mode') == 'directory') then
   lfs.mkdir("data")
end

--Prep torch nn compatible table of tensors from csv.

--Preprocess input with col wise standardization

tr = csvigo.load({path = "/Users/tu/Documents/MLProject/datasources/hedge/numerai_training_data.csv", mode="raw"})

--to = csvigo.load({path = "/Users/tu/Documents/MLProject/datasources/hedge/numerai_tournament_data.csv", mode="raw"})

--kill header
table.remove(tr, 1)
--table.remove(to, 1)

--convert to tensor
tr_ten = torch.Tensor(tr)

mini_tr_ten = torch.Tensor(tr)
mini_tr_ten = mini_tr_ten:narrow(1, 1, 10000)

--to_ten = torch.Tensor(to)

--Implicit shuffle then split.
p = 0.85
train, test = csvtonn.splitTrainTest(tr_ten, p)
mini_train, mini_test = csvtonn.splitTrainTest(mini_tr_ten, p)

--separate into data and labels, this just comes from observing data
start_features = 1
num_features = 21
start_labels = 22
num_labels = 1
num_classes = 2

--Separate single tensor into data and labels tensor.
train = csvtonn.splitDataLabels(train, start_features, num_features, start_labels, num_labels)
test = csvtonn.splitDataLabels(test, start_features, num_features, start_labels, num_labels)

mini_train = csvtonn.splitDataLabels(mini_train, start_features, num_features, start_labels, num_labels)
mini_test = csvtonn.splitDataLabels(mini_test, start_features, num_features, start_labels, num_labels)

--tournament = csvtonn.splitDataLabels(to_ten, start_features, num_features, nil, nil)
--add size and override index operation
--override index method to give {data, label}
train = csvtonn.prepForNN(train, num_classes)
test = csvtonn.prepForNN(test, num_classes)

mini_train = csvtonn.prepForNN(mini_train, num_classes)
mini_test = csvtonn.prepForNN(mini_test, num_classes)


--tournament = csvtonn.prepForNN(tournament, nil)

stat.standardize_data(train)
stat.standardize_data(test)

local opt = models.get_opt()
print(opt)
opt.model = "mlp"
opt.batchSize = 1000
models.set_opt(opt)
models.run(train,test)

