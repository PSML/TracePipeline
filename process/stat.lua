local stat = {}

function stat.standardize_data(t)
   local geo = {}
   geo[1] = t.data:size(1)
   geo[2] = t.data:size(2)

   local t_mean = t.data:mean(1)
   local t_std = t.data:std(1)

--   local rer = t.data - t_mean:expand(geo[1],geo[2])
--   local asw = torch.cdiv(rer, t_std:expand(geo[1], geo[2]))
--   print(asw:size())
--   os.exit()
   t.data = torch.cdiv( (t.data - t_mean:expand(geo[1],geo[2])), t_std:expand(geo[1], geo[2]))
end

function stat.print_lab_dist(t)
   --Assumes binary classification, 0 or 1.
   local total = t:size(1)
   print("of " .. total ..  " examples,")
   local num_one = t.labels:sum()
   local num_zero = total - num_one
   print(num_zero .. " are zero")
   print(num_one .. " are one" )
end

return stat