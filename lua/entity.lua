function valid(o)
  if (type(o) == 'table' or type(o) == 'userdata') and o.is_valid then
    return o:is_valid()
  end

  return false
end
