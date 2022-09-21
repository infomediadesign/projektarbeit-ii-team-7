function math.round(n)
  return math.floor(n * 1000) * 0.001
end

function math.clamp(n, min, max)
  if n > max then
    return max
  elseif n < min then
    return min
  end

  return n
end
