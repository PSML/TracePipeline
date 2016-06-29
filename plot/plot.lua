require 'gnuplot'
require 'csvigo'

function demo()
   LondonTemp = torch.Tensor{{9, 10, 12, 15, 18, 21, 23, 23, 20, 16, 12, 9},
      {5,  5,  6,  7, 10, 13, 15, 15, 13, 10,  7, 5}}
   gnuplot.plot({'High [°C]',LondonTemp[1]},{'Low [°C]',LondonTemp[2]})
   gnuplot.raw('set xtics ("Jan" 1, "Feb" 2, "Mar" 3, "Apr" 4, "May" 5, "Jun" 6, "Jul" 7, "Aug" 8, "Sep" 9, "Oct" 10, "Nov" 11, "Dec" 12)')
   gnuplot.plotflush()
   gnuplot.axis{0,13,0,''}
   gnuplot.grid(true)
   gnuplot.title('London average temperature')
end

function baseline(file)
   local c = csvigo.load({path = file, mode="raw"})
   local c_ten = torch.Tensor(c)   

   c_ten = c_ten:narrow(2,4,1)
   print(#c_ten)
   gnuplot.plot(c_ten)
end


for i=0,9 do
   baseline("data/errf100minus100xabsdiff" .. i .. ".txt")
   io.read()
end



io.read()


