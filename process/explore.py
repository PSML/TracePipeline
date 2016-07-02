--Tools for exploring data
local ex = {}

function ex.num_entries(t)
   return #t
end

function ex.num_features(t)
   return #t[1]
end

return ex