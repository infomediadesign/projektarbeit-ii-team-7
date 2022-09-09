function table.getkeys(t)
  local keys = {}

  for k, _v in pairs(t) do
    table.insert(keys, k)
  end

  return keys
end

function table.length(t)
  local len = 0

  for _k, _v in pairs(t) do
    len = len + 1
  end

  return len
end

function table.hasvalue(t, val)
  for _, v in pairs(t) do
    if v == val then
      return true
    end
  end

  return false
end

function table.hasvaluei(t, val)
  for _, v in ipairs(t) do
    if v == val then
      return true
    end
  end

  return false
end

function printtable(t, indent, done, indent_length)
  done = done or {}
  indent = indent or 0
  indent_length = indent_length or 1

  local keys = table.getkeys(t)

  for k, v in pairs(keys) do
    local l = tostring(v):len()

    if l > indent_length then
      indent_length = l
    end
  end

  indent_length = indent_length + 1

  table.sort(keys, function(a, b)
    if type(a) == "number" and type(b) == "number" then return a < b end
    return tostring(a) < tostring(b)
  end)

  done[t] = true

  for i = 1, #keys do
    local key = keys[i]
    local value = t[key]
    local value_type = type(value)
    io.write(string.rep('  ', indent))

    if value_type == "table" and not done[value] then
      local str_key = tostring(key)

      if value.class or value.class_name then
        io.write(
          str_key..':'..
          string.rep(' ', indent_length - str_key:len())..
          ' #<'..tostring(value.class_name or key)..': '..
          tostring(value):gsub('table: ', '')..'>\n'
        )
      elseif table.length(value) == 0 then
        io.write(str_key..':'..string.rep(' ', indent_length - str_key:len())..' []\n')
      else
        done[value] = true
        io.write(str_key..':\n')
        printtable(value, indent + 1, done, indent_length - 3)
        done[value] = nil
      end
    else
      local str_key = tostring(key)
      io.write(str_key..string.rep(' ', indent_length - str_key:len())..'= ' )

      if value_type == "string" then
        io.write('"'..value..'"\n')
      elseif value_type == "function" then
        io.write('function ('..tostring(value):gsub('function: ', '')..')\n')
      elseif value_type == "table" and (value.class or value.class_name) then
        io.write('#<'..tostring(value.class_name or key)..': '..tostring(value):gsub('table: ', '')..'>\n')
      else
        io.write(tostring(value)..'\n')
      end
    end
  end
end
