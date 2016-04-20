
--This is the loss function, MSE.
criterion = nn.MSECriterion()  
--This is the training technique, SGD.
trainer = nn.StochasticGradient(mlp, criterion)
--Default learning rate is 0.01
--trainer.learningRate = 0.01
--trainer.maxIteration = 100

--Train, does 25 rounds by default
trainer:train(dataset);

