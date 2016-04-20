This will contain code for manipulating bmps into torch friendly tables, which can be fed directly into nns. 

If I knew what I were doing, I would load these bmps directly into a torch table; unfortunately, I don't. I'll workaround this by loading my binary data into a matlab 3d matrix and strip out one channel. I'll store 

D = #bmps

D .bmp files ->
matlab D x 3d matrix ->
#Take 1 of 3 color channels
matlab D x 2d matrix ->
w